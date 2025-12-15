aPackageInfo = [
	:name = "RingHTML",
	:description = "A high-performance HTML5 parser with CSS selectors and DOM manipulation for the Ring programming language.",
	:folder = "ring-html",
	:developer = "ysdragon",
	:email = "",
	:license = "MIT License",
	:version = "1.0.2",
	:ringversion = "1.24",
	:versions = 	[
		[
			:version = "1.0.2",
			:branch = "master"
		]
	],
	:libs = 	[
		
	],
	:files = 	[
		"CMakeLists.txt",
		"LICENSE",
		"README.md",
		"lib.ring",
		"main.ring",
		"package.ring",
		"src/c_src/ring_html.c",
		"src/html.ring",
		"src/utils/color.ring",
		"src/utils/install.ring",
		"src/utils/uninstall.ring",
		"examples/01_basic_parsing.ring",
		"examples/02_css_selectors.ring",
		"examples/03_content_extraction.ring",
		"examples/04_dom_navigation.ring",
		"examples/05_advanced_selectors.ring",
		"examples/06_real_world_scraping.ring",
		"examples/07_xml_parsing.ring",
		"examples/08_dom_manipulation.ring",
		"examples/09_unicode_international.ring",
		"img/logo.png"
	],
	:ringfolderfiles = 	[

	],
	:windowsfiles = 	[
		"lib/windows/i386/ring_html.dll",
		"lib/windows/amd64/ring_html.dll",
		"lib/windows/arm64/ring_html.dll"
	],
	:linuxfiles = 	[
		"lib/linux/amd64/libring_html.so",
		"lib/linux/arm64/libring_html.so"
	],
	:ubuntufiles = 	[

	],
	:fedorafiles = 	[

	],
	:freebsdfiles = 	[
		"lib/freebsd/amd64/libring_html.so",
		"lib/freebsd/arm64/libring_html.so"
	],
	:macosfiles = 	[
		"lib/macos/amd64/libring_html.dylib",
		"lib/macos/arm64/libring_html.dylib"
	],
	:windowsringfolderfiles = 	[

	],
	:linuxringfolderfiles = 	[

	],
	:ubunturingfolderfiles = 	[

	],
	:fedoraringfolderfiles = 	[

	],
	:freebsdringfolderfiles = 	[

	],
	:macosringfolderfiles = 	[

	],
	:run = "ring main.ring",
	:windowsrun = "",
	:linuxrun = "",
	:macosrun = "",
	:ubunturun = "",
	:fedorarun = "",
	:setup = "ring src/utils/install.ring",
	:windowssetup = "",
	:linuxsetup = "",
	:macossetup = "",
	:ubuntusetup = "",
	:fedorasetup = "",
	:remove = "ring src/utils/uninstall.ring",
	:windowsremove = "",
	:linuxremove = "",
	:macosremove = "",
	:ubunturemove = "",
	:fedoraremove = ""
]