#include "Image.h"

void PrintHelp() {
    std::cout << "Example: ./igotpicturesinmyterminal <file_name> <width of terminal>\n";
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cout << "Error!\n";
        PrintHelp();
        return 1;
    } else {
        system("clear");
        try {
            Image img = Image(argv[1]);
            float ar = static_cast<float>(img.Height()) / img.Width();
            int w = 120;

            if (argc == 3) w = std::stoi(argv[2]);

            auto resized = img.Resize(w, ar * 0.6 * w);
            resized.Show();
//            resized.Write();

        } catch (const std::runtime_error& e) {
            std::cerr << e.what();
            PrintHelp();
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << e.what();
            PrintHelp();
            return 1;
        }
    }
    return 0;
}