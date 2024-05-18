#include <cstdlib>
#include <iostream>

void playMP3(const std::string &filePath) {
    std::string command = "cvlc --play-and-exit " + filePath;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to play the MP3 file." << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path/to/your/file.mp3>" << std::endl;
        return 1;
    }

    std::string mp3File = argv[1];
    playMP3(mp3File);
    return 0;
}
