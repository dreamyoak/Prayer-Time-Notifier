# Prayer Time Alert

## Description

Prayer Time Alert is a C++ program that fetches and displays daily prayer times based on your geographical location. The program uses APIs to obtain prayer times and provides real-time updates and notifications for upcoming prayers.

![Prayer Time Alert Screenshot](https://cdn.discordapp.com/attachments/1119213964895715362/1144248592870154290/G1fRSoF.jpg)

## How the API Works

The program follows these steps to retrieve and display prayer times:

1. It starts by fetching your IP address using the [ipinfo.io](https://ipinfo.io) API.
2. Once it has your IP, it sends the IP to the `https://api.dreamyoak.xyz/prayer/` API endpoint.
3. The server behind the API uses your IP to determine your geographical location (city and country).
4. It then fetches the prayer times for your location from [IslamicFinder](https://www.islamicfinder.org/) or another data source.
5. The server compiles the prayer times into a JSON format.
6. The program receives the JSON response from the API, which contains your city, country, and the prayer times.
7. The program displays the prayer times, calculates the time left for the next prayer, and provides real-time updates.
   
The API endpoint used for fetching prayer times is `https://api.dreamyoak.xyz/prayer/`, and the format of the endpoint with your IP address would be:

```
https://api.dreamyoak.xyz/prayer/YOUR_IP_ADDRESS
```

For example, if your IP address is `128.180.76.101`, the full API URL would be:

```
https://api.dreamyoak.xyz/prayer/128.180.76.101
```

## Features

- Fetches prayer times from an API based on your IP location.
- Displays the prayer times in a formatted table.
- Calculates and displays the time left for the next prayer in hours, minutes, and seconds.
- Shows a popup notification when it's time for the next prayer.
- Utilizes libraries such as `nlohmann/json` and `curl` for API requests and JSON parsing.

## Requirements

- C++ compiler (C++11 or later)
- `curl` library
- `nlohmann/json` library

## Installation

1. Clone this repository using the following command:
   
```
git clone https://github.com/dreamyoak/Prayer-Time-Notifier.git
```

2. Navigate to the project directory:

```
cd prayer-time-alert
```

3. Install the required libraries:

- `nlohmann/json`:
  Follow the installation instructions in the [nlohmann/json repository](https://github.com/nlohmann/json#package-managers).

- `curl`:
  On Windows-based systems, you can install the `libcurl` package using the following command:

  ```
  vcpkg install curl
  ```

## Usage

1. Compile the program using a C++ compiler:

```
g++ -o PrayerTimeAlert main.cpp -lcurl
```


2. Run the compiled executable:
   
```
./PrayerTimeAlert
```


3. The program will continuously update and display the prayer times along with the time left for the next prayer. When it's time for the next prayer, a popup notification will appear.

## Screenshots

![Prayer Time Alert Screenshot](https://cdn.discordapp.com/attachments/1119213964895715362/1144248592870154290/G1fRSoF.jpg)

## Credits

This program was developed by [Dreamyoak](https://dreamyoak.xyz) as a project for personal practice and learning.

## Disclaimer

This program relies on external APIs to fetch prayer times. The accuracy of the prayer times depends on the API's data source. Please verify the accuracy of the data if needed.

## License

This project is licensed under the [MIT License](LICENSE).

