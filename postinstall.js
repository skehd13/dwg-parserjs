const { spawn, execSync } = require("child_process");
const os = require("os");

execSync("cd libredwg");
if (os.platform() === "win32") {
  execSync(
    "cmake -G 'Visual Studio 16 2019' -A x64 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ."
  );
  execSync("cmake --build . --config Release");
  execSync("cmake -install .");
} else if (os.platform() === "darwin") {
  execSync("./autogen.sh");
  execSync("cmake .");
  execSync("make");
  execSync("make install");
}
