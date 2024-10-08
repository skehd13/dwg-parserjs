"use strict";
const addon = require("bindings")("reDwg");
function parseDWG(filePath, layerName = []) {
  const result = addon.parseDWG(filePath, layerName);
  return result;
}
function getLayer(filePath) {
  const result = addon.getLayer(filePath);
  return result;
}
module.exports = { parseDWG, getLayer };
