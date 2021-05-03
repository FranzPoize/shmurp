from conans import ConanFile, CMake, tools

from os import path


class ShmurpConan(ConanFile):
    name = "shmurp"
    version = "local"
    license = "The Unlicense"
    author = "adnn"
    url = "https://github.com/Adnn/shmurp"
    description = "Shoot them urp!"
    topics = ("opengl", "2D", "game")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
    }
    default_options = {
        "shared": False,
        "boost:layout": "system", #Should be system on non-Windows
        "glad:api_version": "4.1",
        "glad:extensions": "GL_KHR_debug, GL_ARB_texture_storage",
    }

    requires = (
        ("glad/0.1.29@bincrafters/stable"),
        ("boost/1.75.0"),

        ("aunteater/local"),
        ("graphics/local"),
        ("math/local"),
        ("websocket/local"),
    )

    build_requires = ("cmake_installer/[>=3.16]@conan/stable",)

    generators = "cmake_paths", "cmake"
    build_policy = "missing"

    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
        "submodule": "recursive",
    }


    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_PROJECT_Shmurp_INCLUDE"] = \
            path.join(self.source_folder, "cmake", "conan", "customconan.cmake")
        cmake.definitions["Boost_USE_STATIC_LIBS"] = not self.options["boost"].shared
        cmake.configure()
        return cmake


    def build(self):
        cmake = self._configure_cmake()
        cmake.build()


    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
