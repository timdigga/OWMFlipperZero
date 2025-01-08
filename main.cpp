#include <string>
#include <cstring>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

using namespace std;

const int SAMPLE_RATE = 2000000;
const int SYMBOL_RATE = 2000;

uint8_t nibbles[9];
vector<int> data;
vector<uint8_t> out_cu8;
vector<int8_t> out_cs8;

// ANSI escape codes for color
#define RESET_COLOR   "\033[0m"
#define BOLD_TEXT     "\033[1m"
#define BLUE_TEXT     "\033[34m"
#define CYAN_TEXT     "\033[36m"
#define GREEN_TEXT    "\033[32m"

// Buffer to hold the HTTP response
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Write callback for libcurl
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = (char *)realloc(mem->memory, mem->size + real_size + 1);
    if (!ptr) {
        fprintf(stderr, "Out of memory\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = 0;

    return real_size;
}

void usage(const char *cmd) {
    fprintf(stderr, "Usage: %s [-o file] [-i ID] [-c channel] [-t temp] [-h humidity] [-k api_key] [-l city_name]\n", cmd);
    exit(1);
}

void add_sync(vector<int> &v) {
    v.push_back(1);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
}

void add_zero(vector<int> &v) {
    v.push_back(1);
    v.push_back(0);
    v.push_back(0);
}

void add_one(vector<int> &v) {
    v.push_back(1);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
    v.push_back(0);
}

void generate_data() {
    for (int k = 0 ; k < 12 ; k++) {
        add_sync(data);
        for (int i = 0 ; i < 9 ; i++) {
            uint8_t mask = 0x08;
            for (int j = 0 ; j < 4 ; j++) {
                if (nibbles[i] & mask) {
                    add_one(data);
                } else {
                    add_zero(data);
                }
                mask >>= 1;
            }
        }
    }
}

void generate_samples() {
    int spb = SAMPLE_RATE / SYMBOL_RATE; // samples per bit
    for (int i = 0 ; i < (int)data.size() ; i++) {
        for (int j = 0 ; j < spb ; j++) {
            out_cu8.push_back(data[i] ? 255 : 127);
            out_cu8.push_back(127);
            out_cs8.push_back(data[i] ? 127 : 0);
            out_cs8.push_back(0);
        }
    }
}

void save_sub(const string &fname, int freq) {
    printf("Saving to %s\n", fname.c_str());
    FILE *f = fopen(fname.c_str(), "w");
    fprintf(f, "Filetype: Flipper SubGhz RAW File\n"
               "Version: 1\n"
               "Frequency: %d\n"
               "Preset: FuriHalSubGhzPresetOok650Async\n"
               "Protocol: RAW", freq);

    int one_len = 500;
    int zero_len = 500;
    int prev_bit = -1;
    int prev_len = 0;
    vector<int> raw_data;
    for (int i = 0; i < (int)data.size(); i++) {
        if (prev_bit >= 0 && prev_bit != data[i]) {
            raw_data.push_back(prev_len);
            prev_len = 0;
        }
        if (data[i]) {
            prev_len += one_len;
        } else {
            prev_len -= zero_len;
        }
        prev_bit = data[i];
    }
    raw_data.push_back(prev_len);
    if (prev_bit) {
        raw_data.push_back(-zero_len);
    }
    for (int i = 0; i < (int)raw_data.size(); i++) {
        if (i % 512 == 0) {
            fprintf(f, "\nRAW_Data: ");
        }
        fprintf(f, "%d ", raw_data[i]);
    }
    fclose(f);
}

template<typename T>
void save_to_file(const string &fname, vector<T> &out) {
    printf("Saving to %s\n", fname.c_str());
    FILE *f = fopen(fname.c_str(), "wb");
    fwrite(out.data(), 1, out.size(), f);
    printf("Saved!");
    fclose(f);
}

void fetch_weather_data(const string &api_key, const string &city_name, float &temp, int &humidity) {
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = (char *)malloc(1);
    chunk.size = 0;

    string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city_name + "&appid=" + api_key + "&units=metric";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            cJSON *json = cJSON_Parse(chunk.memory);
            if (json) {
                cJSON *main = cJSON_GetObjectItem(json, "main");
                if (main) {
                    cJSON *temp_item = cJSON_GetObjectItem(main, "temp");
                    cJSON *humidity_item = cJSON_GetObjectItem(main, "humidity");
                    if (temp_item && humidity_item) {
                        temp = temp_item->valuedouble;
                        humidity = humidity_item->valueint;
                    }
                }
                cJSON_Delete(json);
            }
        }
        curl_easy_cleanup(curl);
    }
    free(chunk.memory);
}

int main(int argc, char *argv[]) {
    int opt;
    string api_key, city_name, fname;
    int freq = 433920000;  // Set frequency to 433.92 MHz
    float temp = 26.3; // Default temperature
    int humidity = 20; // Default humidity
    uint8_t id = 244;
    int8_t channel = 1;

    printf("\n");
    printf(BLUE_TEXT "---------------------------\n" RESET_COLOR);
    printf(GREEN_TEXT "- " BOLD_TEXT "Weather Station Spoofer" RESET_COLOR " -\n");
    printf(BLUE_TEXT "---------------------------\n\n" RESET_COLOR);

    while ((opt = getopt(argc, argv, "i:c:t:h:o:k:l:f:")) != -1) {
        switch (opt) {
            case 'k':
                api_key = optarg;
                break;
            case 'l':
                city_name = optarg;
                break;
            case 'o':
                fname = optarg;
                break;
            default:
                usage(argv[0]);
        }
    }

    if (!api_key.empty() && !city_name.empty()) {
        fetch_weather_data(api_key, city_name, temp, humidity);
        printf("Fetched weather data: Temp = %.2f°C, Humidity = %d%%\n", temp, humidity);
    }

    int16_t temp_encoded = (int16_t)(temp * 10);

    nibbles[0] = (id >> 4) & 0x0f;
    nibbles[1] = id & 0x0f;
    nibbles[2] = 7 + channel;
    nibbles[3] = (temp_encoded >> 8) & 0x0f;
    nibbles[4] = (temp_encoded >> 4) & 0x0f;
    nibbles[5] = temp_encoded & 0x0f;
    nibbles[6] = 0x0f;
    nibbles[7] = (humidity >> 4) & 0x0f;
    nibbles[8] = humidity & 0x0f;

    generate_data();
    generate_samples();

    if (!fname.empty()) {
        save_sub(fname + ".sub", freq);
        printf("Saved .sub files!");
        return 0;
    }

    fprintf(stderr, "No output file specified. Use the -o option to specify a file name.\n");
    return 1;
}
