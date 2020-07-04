#include "scene_mainmenu.hpp"

#include "ember/engine.hpp"
#include "ember/config.hpp"
#include "ember/emberjs/config.hpp"

#include <emscripten.h>
#include <emscripten/html5.h>

#include <iostream>
#include <stdexcept>
#include <cstddef>
#include <functional>

void print_exception(const std::exception& e, int level = 0) {
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception &e) {
        print_exception(e, level + 1);
    } catch (...) {
    }
}

std::function<void()> loop;
void main_loop() try {
    loop();
} catch (const std::exception& e) {
    std::cerr << "Fatal exception:" << std::endl;
    print_exception(e, 1);
    std::terminate();
}

int main() try {
    std::cout << "Init..." << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(SDL_GetError());
    }

    std::cout << "Loading config..." << std::endl;

    auto config = emberjs::get_config().get<ember::config::config>();

    std::cout << "Instantiating engine..." << std::endl;

    auto engine = std::make_unique<ember::engine>(config);

    engine->queue_transition<scene_mainmenu>();

    std::cout << "Success." << std::endl;

    loop = [&engine]{ engine->tick(); };

    emscripten_set_main_loop(main_loop, 0, 1);

    SDL_Quit();

    return EXIT_SUCCESS;
} catch (const std::exception& e) {
    std::cerr << "Fatal exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
