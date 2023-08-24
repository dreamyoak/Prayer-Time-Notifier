#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <windows.h>

using json = nlohmann::json;

void showPopupMessage(const std::string &message) {
    MessageBoxA(NULL, message.c_str(), "Prayer Time Alert", MB_OK);
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

std::string removeQuotes(const std::string &input) {
    std::string result = input;
    result.erase(std::remove(result.begin(), result.end(), '\"'), result.end());
    return result;
}

std::string getCurrentTime12HourFormat() {
    time_t currentTime = time(nullptr);
    struct tm *timeinfo = localtime(&currentTime);

    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%I:%M %p", timeinfo);

    return std::string(timeBuffer);
}


std::string convert12HourTo24Hour(const std::string &time12Hour) {
    std::string time24Hour;
    int hour, minute;
    char ampm[3];

    if (sscanf(time12Hour.c_str(), "%d:%d %2s", &hour, &minute, ampm) != 3) {
        std::cerr << "Failed to parse time12Hour: " << time12Hour << "\n";
        return ""; 
    }

    if (strcmp(ampm, "PM") == 0 && hour != 12) {
        hour += 12;
    } else if (strcmp(ampm, "AM") == 0 && hour == 12) {
        hour = 0;
    }

    char timeBuffer[80];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", hour, minute);

    return std::string(timeBuffer);
}

int calculateTimeDifferenceInSeconds(const std::string& time1, const std::string& time2) {
    std::istringstream ss1(time1);
    std::istringstream ss2(time2);

    int hours1, minutes1, hours2, minutes2;
    char delimiter;

    ss1 >> hours1 >> delimiter >> minutes1;
    ss2 >> hours2 >> delimiter >> minutes2;

    int totalMinutes1 = hours1 * 60 + minutes1;
    int totalMinutes2 = hours2 * 60 + minutes2;

    int timeDifferenceInMinutes = totalMinutes2 - totalMinutes1;
    int timeDifferenceInSeconds = timeDifferenceInMinutes * 60;

    return timeDifferenceInSeconds;
}

void printPrayerTable(const json& prayerTimes) {
    const std::vector<std::string> prayerOrder = {"Fajr", "Dhuhr", "Asr", "Maghrib", "Isha"};

    std::cout << "\u001b[35m╔═════════╦══════════╗" << "\n";
    std::cout << "║ \u001b[36mPrayer  \u001b[35m║   \u001b[36mTime   \u001b[35m║" << "\n";
    std::cout << "║         ║          ║" << "\n";

    for (const std::string &prayerName : prayerOrder) {
        if (prayerTimes.contains(prayerName)) {
            std::string prayerTime = removeQuotes(prayerTimes[prayerName]);
            std::cout << "\u001b[35m║ \u001b[36m" << std::left << std::setw(7) << prayerName << "\u001b[35m ║ \u001b[36m" << std::setw(8) << prayerTime << " \u001b[35m║\u001b[0m" << "\n";
        }
    }
    std::cout << "\u001b[35m╚═════════╩══════════╝" << "\n\u001b[0m";
}

std::string fetchAPIData(const std::string &url) {
    std::string apiResponse;

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &apiResponse);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to fetch data from API: " << curl_easy_strerror(res) << "\n";
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return apiResponse;
}

int main() {
    std::thread apiThread;
    SetConsoleOutputCP( 65001 );

    while (true) {
        system("cls");
        try {
            std::string apiResponse;
            apiResponse = fetchAPIData("https://api.dreamyoak.xyz/prayer/" + fetchAPIData("https://ipinfo.io/ip"));

            json data = json::parse(apiResponse);
            std::string city = data["city"];
            std::string country = data["country"];
            json prayerTimes = data["prayer_times"];

            std::cout << "\u001b[31m" << city << "\u001b[37m - \u001b[31m" << country << "\n\u001b[0m";

            const std::vector<std::string> prayerOrder = {"Fajr", "Dhuhr", "Asr", "Maghrib", "Isha"};
            printPrayerTable(prayerTimes);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::string currentTime = convert12HourTo24Hour(getCurrentTime12HourFormat());

            std::string nextPrayerName;
            std::string nextPrayerTime;
            std::string nextPrayerTime12H;
            bool foundNextPrayer = false;

            while (!foundNextPrayer) {
                for (const std::string &prayerName : prayerOrder) {
                    if (prayerTimes.contains(prayerName)) {
                        std::string prayerTime = removeQuotes(prayerTimes[prayerName]);

                        if (currentTime < convert12HourTo24Hour(prayerTime)) {
                            nextPrayerName = prayerName;
                            nextPrayerTime12H = prayerTime;
                            nextPrayerTime = convert12HourTo24Hour(prayerTime);
                            foundNextPrayer = true;
                            break;
                        }
                    }
                }

                if (!foundNextPrayer) {
                    std::this_thread::sleep_for(std::chrono::minutes(1)); 
                    apiThread.join();
                    apiThread = std::thread(fetchAPIData, std::ref(apiResponse));
                }
            }

            int timeLeftInSeconds = calculateTimeDifferenceInSeconds(currentTime, nextPrayerTime);
            int hoursLeft = timeLeftInSeconds / 3600;
            int minutesLeft = (timeLeftInSeconds % 3600) / 60;
            int secondsLeft = timeLeftInSeconds % 60;

            while (timeLeftInSeconds > 0) {
                std::cout << "Next Prayer: \u001b[31m" << nextPrayerName << " at " << nextPrayerTime12H << "\u001b[0m - " 
                << "Time left: \u001b[31m" << hoursLeft << "h " << minutesLeft << "m " << secondsLeft << "s" << "\u001b[0m - Time Now: \u001b[31m" 
                << getCurrentTime12HourFormat() << "\r\u001b[0m";

                std::this_thread::sleep_for(std::chrono::seconds(1));
                timeLeftInSeconds--;
                hoursLeft = timeLeftInSeconds / 3600;
                minutesLeft = (timeLeftInSeconds % 3600) / 60;
                secondsLeft = timeLeftInSeconds % 60;
            }

            std::cout << "Next Prayer: " << nextPrayerName << " at " << nextPrayerTime << " - ";
            std::cout << "Time left: 0h 0m 0s" << "\n";
            showPopupMessage("Time to Pray " + nextPrayerName + "\n" + getCurrentTime12HourFormat());
        } catch (const std::exception &e) {
            std::cerr << "An exception occurred: " << e.what() << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
    }

    curl_global_cleanup();
    return 0;
}