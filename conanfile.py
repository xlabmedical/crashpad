from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.microsoft import is_msvc
from conan.tools.files import copy
from conan.tools.cmake import CMakeDeps
from conan.tools.env import Environment
import os
import shutil

class CompressorRecipe(ConanFile):
	# Binary configuration
	settings = "os", "compiler", "build_type", "arch"
	generators = "CMakeDeps"
	python_requires = "requirements/0.3@xlab/stable"
	python_requires_extend = "requirements.RequirementsBase"

	def requirements(self):
		self.requires("qt/5.12.6@xlab/stable#0bc68af1ca2666653c18ab000f7138add73d9d48", force=True)
		self.requires("openssl/1.1.1q#d6d3d8423b092aabec30e8193a7ca29d", force=True)
		# self.requires("libarchive/3.6.0#9341f78645f7adfe957e853c1528284e", force=True)
	def generate(self):
		super().generate()

		# Additional RealGUIDE specific generate
		if not is_msvc(self):
			# Copy 3dconenxionclient framework
			copy(self, "*", os.path.join(self.dependencies["qt"].package_folder, "plugins") , os.path.join(self.build_folder, "bin/"))


