# OWMFlipperZero
Use a OpenWeatherMap API to sync your Weather Station
Features of This Version

	1.	Weather Data Fetching:
	•	Use -k <API_KEY> and -l <CITY_NAME> to fetch real-time weather data (temperature and humidity).
	2.	Encoding Weather Data:
	•	Encodes fetched weather data into .sub files for Flipper Zero.
	3.	Defaults:
	•	If API key or city is not provided, uses hardcoded values.

Example Usage

./weather_spoofer -k YOUR_API_KEY -l "London" -o output

This will fetch weather data for London, encode it, and save it as output.sub.