/*
	Ring HTML Library Install Script
	----------------------------------
	Installs Ring HTML library for the current platform.
	Detects OS and architecture, then copies or symlinks the library
	to the appropriate system location.
*/

load "stdlibcore.ring"
load "src/utils/color.ring"

# ============================================================================
# Constants
# ============================================================================

C_PRETTY_NAME      = "Ring HTML"
C_PACKAGE_NAME     = "ring-html"
C_NEW_PACKAGE_NAME = "html"
C_LIB_NAME         = "ring_html"
C_SAMPLES_DIR      = "UsingHTML"

# ============================================================================
# Main Entry Point
# ============================================================================

func main
	new Installer()

# ============================================================================
# Installer Class
# ============================================================================

class Installer

	# Platform configuration
	cOSName    = ""
	cArchName  = ""
	cLibPrefix = ""
	cLibExt    = ""
	cPathSep   = "/"

	# Paths
	cPackagePath  = ""
	cLibPath      = ""
	cSamplesPath  = ""
	cExamplesPath = ""

	func init
		if not detectPlatform()
			return
		ok

		if not detectArchitecture()
			return
		ok

		initializePaths()

		if not verifyLibrary()
			return
		ok

		performInstallation()

	# ========================================================================
	# Platform Detection
	# ========================================================================

	func detectPlatform
		if isWindows()
			configurePlatform("windows", "", ".dll", "\\")
		but isLinux()
			configurePlatform("linux", "lib", ".so", "/")
		but isFreeBSD()
			configurePlatform("freebsd", "lib", ".so", "/")
		but isMacOSX()
			configurePlatform("macos", "lib", ".dylib", "/")
		else
			printError("Unsupported operating system detected!")
			return false
		ok
		return true

	func configurePlatform osName, libPrefix, libExt, pathSep
		cOSName    = osName
		cLibPrefix = libPrefix
		cLibExt    = libExt
		cPathSep   = pathSep

	func detectArchitecture
		cArchName = getarch()

		switch cArchName
			on "x86"
				cArchName = "i386"
			on "x64"
				cArchName = "amd64"
			on "arm64"
			other
				printError("Unsupported architecture: " + cArchName)
				return false
		off

		return true

	# ========================================================================
	# Path Configuration
	# ========================================================================

	func initializePaths
		cPackagePath = buildPath([
			exefolder(), "..", "tools", "ringpm", "packages", C_PACKAGE_NAME
		])

		cLibPath = buildPath([
			cPackagePath, "lib", cOSName, cArchName,
			cLibPrefix + C_LIB_NAME + cLibExt
		])

		cExamplesPath = buildPath([cPackagePath, "examples"])
		cSamplesPath  = buildPath([exefolder(), "..", "samples", C_SAMPLES_DIR])

	func verifyLibrary
		if fexists(cLibPath)
			return true
		ok

		printError(C_PRETTY_NAME + " library not found!")
		printWarning("Expected location: " + cLibPath)
		printInfo("Ensure the library is built for: " + cOSName + "/" + cArchName)
		printInfo("Build instructions: " + buildPath([cPackagePath, "README.md"]))
		return false

	# ========================================================================
	# Installation
	# ========================================================================

	func performInstallation
		try
			installLibrary()
			copyExamples()
			updateRingConfig()
			setupRing2EXE()
			showSuccessMessage()
		catch
			printError("Failed to install " + C_PRETTY_NAME + "!")
			printWarning("Details: " + cCatchError)
		done

	func installLibrary
		if isWindows()
			installWindowsLibrary()
		else
			installUnixLibrary()
		ok

	func installWindowsLibrary
		systemSilent('copy /y "' + cLibPath + '" "' + exefolder() + '"')

	func installUnixLibrary
		cRingLibDir = buildPath([exefolder(), "..", "lib"])

		# Determine system library directory
		if isFreeBSD() or isMacOSX()
			cSystemLibDir = "/usr/local/lib"
		else
			cSystemLibDir = "/usr/lib"
		ok

		# Symlink to Ring lib directory
		system('ln -sf "' + cLibPath + '" "' + cRingLibDir + '"')

		# Symlink to system lib directory (with privilege escalation fallback)
		cLinkCmd = 'ln -sf "' + cLibPath + '" "' + cSystemLibDir + '"'
		system(buildElevatedCommand(cLinkCmd))

	func buildElevatedCommand baseCmd
		return 'which sudo >/dev/null 2>&1 && sudo ' + baseCmd +
			   ' || (which doas >/dev/null 2>&1 && doas ' + baseCmd +
			   ' || ' + baseCmd + ')'

	# ========================================================================
	# Examples & Configuration
	# ========================================================================

	func copyExamples
		cOriginalDir = currentdir()

		ensureDirectory(buildPath([exefolder(), "..", "samples"]))
		makeDir(cSamplesPath)
		chdir(cSamplesPath)

		aItems = dir(cExamplesPath)
		if aItems = NULL
			chdir(cOriginalDir)
			return
		ok

		for item in aItems
			cSourcePath = cExamplesPath + cPathSep
			if item[2]
				OSCopyFolder(cSourcePath, item[1])
			else
				OSCopyFile(cSourcePath + item[1])
			ok
		next

		chdir(cOriginalDir)

	func updateRingConfig
		cOldConfigPath = buildPath([exefolder(), C_PACKAGE_NAME + ".ring"])

		if fexists(cOldConfigPath)
			remove(cOldConfigPath)
		ok

		# Ensure load directory exists
		cLoadDir = buildPath([exefolder(), "load"])
		ensureDirectory(cLoadDir)

		cNewConfigPath = buildPath([cLoadDir, C_NEW_PACKAGE_NAME + ".ring"])
		cLoadStatement = 'load "../../tools/ringpm/packages/' + C_PACKAGE_NAME + '/lib.ring"'
		write(cNewConfigPath, cLoadStatement)

	func setupRing2EXE
		cLibsDir = buildPath([exefolder(), "..", "tools", "ring2exe", "libs"])

		if not direxists(cLibsDir)
			return
		ok

		cConfigPath = buildPath([cLibsDir, C_NEW_PACKAGE_NAME + ".ring"])
		write(cConfigPath, generateRing2EXEConfig())

	func generateRing2EXEConfig
		return 'aLibrary = [
	:name         = :' + C_NEW_PACKAGE_NAME + ',
	:title        = "' + C_PRETTY_NAME + '",
	:windowsfiles = ["' + C_LIB_NAME + '.dll"],
	:linuxfiles   = ["lib' + C_LIB_NAME + '.so"],
	:macosxfiles  = ["lib' + C_LIB_NAME + '.dylib"],
	:freebsdfiles = ["lib' + C_LIB_NAME + '.so"],
	:ubuntudep    = "",
	:fedoradep    = "",
	:macosxdep    = ""
]'

	func showSuccessMessage
		printSuccess("Successfully installed " + C_PRETTY_NAME + "!")
		printInfo("Samples available in: " + cSamplesPath)
		printInfo("Package examples: " + cExamplesPath)

	# ========================================================================
	# Utility Methods
	# ========================================================================

	func buildPath aComponents
		cResult = ""
		nCount  = len(aComponents)

		for i = 1 to nCount
			cResult += aComponents[i]
			if i < nCount
				cResult += cPathSep
			ok
		next

		return cResult

	func ensureDirectory cPath
		if not direxists(cPath)
			makeDir(cPath)
		ok