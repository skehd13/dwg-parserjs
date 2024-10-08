const addon = require("bindings")("reDwg");

// function parseDWG(filePath) {
const filePath = "test.dwg";
console.log(filePath);
console.log(addon); // C++ 네이티브 애드온 호출
const result = addon.parseDWG(filePath, []);
// const result = addon.getLayer(filePath);
const data = JSON.stringify(result);
console.log(data);
