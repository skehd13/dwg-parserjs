const addon = require("./build/Release/reDwg");

function parseDWG(filePath: string, layerName: string[] = []) {
  const result = addon.parseDWG(filePath, layerName);

  return result;
}
function getLayer(filePath: string) {
  const result = addon.getLayer(filePath);

  return result;
}

module.exports = { parseDWG, getLayer };
