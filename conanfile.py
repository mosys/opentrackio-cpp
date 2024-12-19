from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps

class opentrackiocppRecipe(ConanFile):
    name = "opentrackio-cpp"
    version = "0.9.1"
    package_type = "library"

    license = "MIT"
    author = "Mo-Sys Engineering Ltd"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "A Cpp helper library for usage with the OpenTrackIO protocol."
    topics = ("OpenTrackIO")

    settings = {"os", "compiler", "build_type", "arch"}
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = "CMakeLists.txt", "src/*", "include/*", "external/*", "cmake/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")
        
        self.settings.compiler.cppstd = 20

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")
            
    def validate(self):
        check_min_cppstd(self, 20)
        
    def requirements(self):
        self.requires("nlohmann_json/3.11.3", transitive_headers=True)
    
    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def package_info(self):
        self.cpp_info.libs = ['opentrackio-cpp']