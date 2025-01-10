# Flipper Zero OpenWeatherMap Temp Spoofer

An enhanced version of [spoof_temp](https://github.com/rgerganov/spoof_temp) designed specifically for the Flipper Zero, with real-time weather data integration using the OpenWeatherMap API. This project allows you to spoof temperature and humidity readings directly on the Flipper Zero, enabling seamless experimentation and testing with live weather data.

# 🚀 Features

- **OpenWeatherMap Integration**: Fetch real-time temperature and humidity data using the OpenWeatherMap API.
- **Flipper Zero Compatibility**: Generate `.sub` files for use with Flipper Zero's Sub-GHz system.
- **Real-Time Weather Data**: Simulate accurate, live weather conditions for testing and fun.
- **Customizable**: Override weather data with custom values as needed.
- **Simple Workflow**: Drag-and-drop generated `.sub` files to your Flipper Zero SD card.

# 📦 Installation

Follow these steps to install and use the program:

# 1. Clone the Repository

```bash
git clone https://github.com/timdigga/flipper-zero-openweathermap-spoofer.git
cd flipper-zero-openweathermap-spoofer

2. Install Required Dependencies

Ensure you have libcurl and libcjson installed:
sudo apt update
sudo apt install libcurl4-openssl-dev libcjson-dev


3. Build the Project

Run the make command to build the program:

g++ main.cpp -o spoof_temp -Wall -std=c++11 -lcurl -lcjson

4. Run the Program

You can now use the program to generate .sub files:

./spoof_temp -o <output_file> -k <your_openweathermap_api_key> -c <channel>

Replace <your_openweathermap_api_key> with your OpenWeatherMap API key. For more usage details, see below.
📋 Usage

The program accepts the following options:
	•	-o <file>: Specify the output file name (e.g., output.sub).
	•	-k <api_key>: Your OpenWeatherMap API key.
	•	-c <channel>: Sub-GHz channel (1, 2, or 3).
	•	-i <ID>: Set the device ID (default: 244).
	•	-f <frequency>: Set the frequency in Hz (default: 433920000).
	•	-t <temperature>: Override the temperature value (in Celsius).
	•	-h <humidity>: Override the humidity value (0–100).
###
Example:

./spoof_temp -o weather.sub -k abc123xyz -c 1

🔄 Workflow with Flipper Zero

	1.	Generate the .sub file using the program.
	2.	Drag and drop the generated .sub file onto your Flipper Zero SD card (e.g., /subghz/).
	3.	Open the Sub-GHz app on your Flipper Zero and navigate to the saved file.
	4.	Transmit the spoofed weather data with ease!

🛠️ Development

If you’d like to contribute or modify the code, feel free to fork the repository and submit pull requests. Suggestions and improvements are always welcome!

📄 License

This project is licensed under the MIT License. See the LICENSE file for details.
