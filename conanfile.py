from conans import ConanFile, AutoToolsBuildEnvironment, tools

class ThorsSerializerConan(ConanFile):
    name        = "ThorsSerializer"
    version     = "2.2.18"
    author      = "Martin York (Aka Loki Astari) Martin.York@gmail.com"
    url         = "git@github.com:Loki-Astari/ThorsSerializer.git"
    license     = "MIT"
    description = """No Code Serialization Library.
                     Supports: JSON YAML BSON"""
    settings    = "cppstd", "os", "compiler", "build_type", "arch"
    scm         = {
        "type"      : "git",
        "url"       : "git@github.com:Loki-Astari/ThorsSerializer.git",
        "revision"  : "33cc9d576aaf9b3695e50c4abf8f254e6fb29581"
    }
    requires = "libyaml/0.2.5", "boost/1.81.0"
    cppminimum = 17

    def configure(self):
        if not self.settings.cppstd:
            self.settings.cppstd = self.cppminimum

    def validate(self):
        tools.valid_min_cppstd(self, self.cppminimum)

    def build_id(self):
        self.info_build.settings.build_type = "Any"

    def build(self):
        atools = AutoToolsBuildEnvironment(self)
        atools.configure(args=["--disable-vera", "--with-standard=%s" % self.settings.cppstd])
        atools.make()

    def package(self):
        atools = AutoToolsBuildEnvironment(self)
        atools.install()

    def package_info(self):
        self.cpp_info.libs = [ "ThorsLogging%s" % self.settings.cppstd, "ThorSerialize%s" % self.settings.cppstd ]
