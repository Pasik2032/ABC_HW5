#include <iostream>
#include <thread>

// Вывод ошибки входных данных.
void errMessage1() {
    std::cout << "The first argument is the length of the plot of land,\n"
                 "the second is the width, the third is the time for which\n"
                 "the first gardener will process one unit of the plot,\n"
                 "the fourth argument is the time of the second gardener\n"
                 "Arguments are entered separated by a space" << std::endl;
}

// Мьютекс отвечающий за блокировку участка земли (двумерного массива).
std::mutex plot;
// Мьютекс отвечающий за блокировку консоли для вывода информации.
std::mutex console;

// Вывод текущего состояние участка земли.
void print(int length, int width, int *const *plotOfLand) {
    std::cout << "-----------------" << std::endl;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < length; ++j) {
            std::cout << plotOfLand[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------" << std::endl;
}

// Работа первого садовника.
void gardenerFistWork(int **plotOfLand, int length, int width, int speed) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < length; ++j) {
            bool flag = false;
            plot.lock();
            if (plotOfLand[i][j] == 0) {
                plotOfLand[i][j] = 1;
                console.lock();
                std::cout << "The first gardener began to process the cage " << i << " " << j << std::endl;
                console.unlock();
                flag = true;
            }
            plot.unlock();
            if (flag) {
                std::this_thread::sleep_for(std::chrono::milliseconds(speed));
                plotOfLand[i][j] = 2;
                console.lock();
                std::cout << "The first gardener finished processing the cage " << i << " " << j << std::endl;
                print(length, width, plotOfLand);
                console.unlock();
            }
        }
    }
}

// Работа второго садовника.
void gardenerSecondWork(int **plotOfLand, int length, int width, int speed) {
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < width; ++j) {
            bool flag = false;
            plot.lock();
            if (plotOfLand[width - j - 1][length - 1 - i] == 0) {
                plotOfLand[width - j - 1][length - 1 - i] = 1;
                console.lock();
                std::cout << "The second gardener began to process the cage " << width - j - 1 << " "
                          << length - 1 - i << std::endl;
                console.unlock();
                flag = true;
            }
            plot.unlock();
            if (flag) {
                std::this_thread::sleep_for(std::chrono::milliseconds(speed));
                plotOfLand[width - j - 1][length - 1 - i] = 2;
                console.lock();
                std::cout << "The second gardener finished processing the cage " << width - j - 1 << " "
                          << length - 1 - i << std::endl;
                print(length, width, plotOfLand);
                console.unlock();
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        errMessage1();
        return 1;
    }
    int length, width, speed_fist, speed_second;
    try {
        length = atoi(argv[1]);
        width = atoi(argv[2]);
        speed_fist = atoi(argv[3]);
        speed_second = atoi(argv[4]);
    } catch (std::exception e) {
        std::cout << "invalid argument values" << std::endl;
        errMessage1();
    }
    int **plotOfLand = new int *[width];
    for (int i = 0; i < width; ++i) {
        plotOfLand[i] = new int[length];
        for (int j = 0; j < length; ++j) {
            plotOfLand[i][j] = 0;
        }
    }
    std::cout << "0 - untreated area" << std::endl;
    std::cout << "1 - gardener in the process of processing" << std::endl;
    std::cout << "2 - the plot is processed" << std::endl;
    print(length, width, plotOfLand);
    std::cout << "Start" << std::endl;
    // Запуск работы первого садовника.
    std::thread gardenerFist(gardenerFistWork, plotOfLand, length, width, speed_fist);
    //Запуск работы второго садовника.
    std::thread gardenerSecond(gardenerSecondWork, plotOfLand, length, width, speed_second);
    gardenerFist.join();
    gardenerSecond.join();
    std::cout << "end" << std::endl;
    return 0;
}
