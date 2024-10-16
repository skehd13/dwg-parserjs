#include <napi.h>
#include <iostream>
#include <dwg.h>
#include <dwg_api.h>
#include <string>

uint32_t zero = 0;
char* checkLayerName;
std::vector<char*> charArray;

std::string parseChar(char* text);
std::string parseChar(unsigned char* text);
std::string parseChar(unsigned char text);
Napi::Object createObject(Napi::Env env);
Napi::Array checkObject(Napi::Object jsonObj, std::string key);
// ��ġ �Ľ�
void parseEntityHatch(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// �ָ��� �Ľ�
void parseEntitySolid(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// Ÿ���Ľ�
void parseEntityARC(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// M�ؽ�Ʈ �Ľ�
// void parseEntityMText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// �ؽ�Ʈ �Ľ�
// void parseEntityText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// ��ġ �Ľ�
void parseEntitySpline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// X�� �Ľ�
void parseEntityXline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// ���� �Ľ�
void parseEntityRay(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// �� �Ľ�
void parseEntityPoint(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// ���̾ƿ� �Ľ�
// void parseLayout(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// ���� �Ľ�
void parseBlock(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// ������� �Ľ�
void parseBlockHeader(Dwg_Object_Object* object_object, Napi::Object jsonObj, Napi::Env env);
// Insert �Ľ�
void parseEntityInsert(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// �� �Ľ�
void parseEntityLine(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// �� �Ľ�
void parseEntityCircle(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// ������ �Ľ�
void parseEntityLwPolyline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// Polyline2D �Ľ�
void parseEntityPolyline2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// Vertex2D �Ľ�
void parseEntityVertex2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// Ÿ��(?) �Ľ�
void parseEntityEllipse(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// ����(?) �Ľ�
// void parseEntityAttdef(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// DIMENSION_LINEAR
// void parseEntityDimensionLinear(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
// SOLID
// void parseEntitySolid(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
std::string ExtractDWGData(Dwg_Data dwg, Napi::Array jsonArr, Napi::Env env);
void parseDWGObject(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env);
std::string CheckObjectRef(Dwg_Data *restrict dwg, Napi::Array jsonArr, Napi::Env env);
char* EntityLayerName(Dwg_Object* object);
std::string CheckLayer(Dwg_Data *restrict dwg, Napi::Object jsonObj, Napi::Env env);
Napi::Value ParseDWG(const Napi::CallbackInfo& info);

bool contains(const std::vector<char*>& charArray, const char* target) {
  if (charArray.empty()) {
        return true;  // ��� ������ false ��ȯ
    }
    // std::find_if�� ����Ͽ� target�� ���� ���ڿ��� ã���ϴ�.
    auto it = std::find_if(charArray.begin(), charArray.end(), 
        [target](const char* str) {
            return strcmp(str, target) == 0;  // ���ڿ� ��
        });

    return it != charArray.end();  // ã���� true, �ƴϸ� false
}

void clearCharArray() {
    for (char* str : charArray) {
        delete[] str; // �� �����ͷ� �Ҵ�� �޸� ����
    }
    charArray.clear(); // ���� ���� (���� ����)
}

bool contains(char* arr[], int length, const char* target) {
    for (int i = 0; i < length; ++i) {
        if (strcmp(arr[i], target) == 0) {
            return true;  // ã���� true ��ȯ
        }
    }
    return false;  // ã�� ���ϸ� false ��ȯ
}

std::string parseChar(char* text) {
  std::string myString;
  
  int size = 0;
  while(true) {
    int i = size;
    if(text[i] == '\0'){
      break;
    }
    if(text[i] == 0){
      break;
    }
    if(text[i] == (char) NULL){
      break;
    }
    size = size+2;
    myString += text[i];
  }
  return myString;
}

std::string parseChar(unsigned char* text) {
  std::string myString;
  myString = static_cast<std::string>(reinterpret_cast<const char *>(text));
  return myString;
}
std::string parseChar(unsigned char text) {
  std::string str;
  str.assign(reinterpret_cast<const char*>(text));
  return str;
}

Napi::Object checkArray(Napi::Array jsonArr, std::string key, Napi::Env env ) {
  bool check = false;
  Napi::Object jsonObj;
  for(unsigned int i=0; i < jsonArr.Length(); i++){
    Napi::Object obj = jsonArr.Get(i).As<Napi::Object>();
    std::string layer_name = obj.Get("layer_name").As<Napi::String>().Utf8Value();
    // std::cout << "check array layer_name:" << layer_name << std::endl;
    if(layer_name == key) {
      jsonObj = jsonArr.Get(i).As<Napi::Object>();
      check = true;
    }
  }
  if(check == false){
    jsonObj = Napi::Object::New(env);
    jsonObj.Set("layer_name", key);
    jsonObj.Set("component", Napi::Object::New(env));
    jsonArr.Set(jsonArr.Length(), jsonObj);
  }
  return jsonObj.Get("component").As<Napi::Object>();
}

Napi::Array checkObject(Napi::Object jsonObj, std::string key, Napi::Env env) {
  Napi::Array array;
  if(jsonObj.Has(Napi::String::New(env, key))) {
    array = jsonObj.Get(key).As<Napi::Array>();
  } else{
    array = Napi::Array::New(env);
  }
  return array;
}

Napi::Object createObject(Napi::Env env) {
  Napi::Object jsonObj = Napi::Object::New(env);
  // Napi::Array line = Napi::Array::New(env);
  Napi::Array circle = Napi::Array::New(env);
  Napi::Array lwPolyline = Napi::Array::New(env);
  Napi::Array block = Napi::Array::New(env);
  Napi::Array viewPosition = Napi::Array::New(env);
  Napi::Array arc = Napi::Array::New(env);
  // Napi::Array mtext = Napi::Array::New(env);
  // Napi::Array text = Napi::Array::New(env);
  // Napi::Array spline = Napi::Array::New(env);
  // Napi::Array xline = Napi::Array::New(env);
  // Napi::Array ray = Napi::Array::New(env);
  Napi::Array point = Napi::Array::New(env);
  Napi::Array hatch = Napi::Array::New(env);
  // Napi::Array blockHeader = Napi::Array::New(env);
  Napi::Array insert = Napi::Array::New(env);
  Napi::Array polyline2D = Napi::Array::New(env);
  Napi::Array vertex2D = Napi::Array::New(env);
  // Napi::Array ellipse = Napi::Array::New(env);
  // Napi::Array attdef = Napi::Array::New(env);
  // Napi::Array dimensionLinear = Napi::Array::New(env);
  // Napi::Array solid = Napi::Array::New(env);

  // jsonObj.Set("line", line);
  jsonObj.Set("circle", circle);
  jsonObj.Set("lwPolyline", lwPolyline);
  jsonObj.Set("block", block);
  jsonObj.Set("viewPosition", viewPosition);
  jsonObj.Set("arc", arc);
  // jsonObj.Set("mtext", mtext);
  // jsonObj.Set("text", text);
  // jsonObj.Set("spline", spline);
  // jsonObj.Set("xline", xline);
  // jsonObj.Set("ray", ray);
  jsonObj.Set("point", point);
  jsonObj.Set("hatch", hatch);
  // jsonObj.Set("blockHeader", blockHeader);
  jsonObj.Set("insert", insert);
  jsonObj.Set("polyline2D", polyline2D);
  jsonObj.Set("vertex2D", vertex2D);
  // jsonObj.Set("ellipse", ellipse);
  // jsonObj.Set("attdef", attdef);
  // jsonObj.Set("dimensionLinear", dimensionLinear);
  // jsonObj.Set("solid", solid);
  return jsonObj;
}

void parseEntityHatch(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_HATCH* hatch = object->tio.entity->tio.HATCH;
  char *layer_name = EntityLayerName(object);
  
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array hatchArray = checkObject(jsonObj, "hatch", env);

  Napi::Object hatchObj = Napi::Object::New(env);
  BITCODE_BL is_gradient_fill = hatch->is_gradient_fill;
  hatchObj.Set("is_gradient_fill", Napi::Number::New(env, is_gradient_fill));
  BITCODE_BL reserved= hatch->reserved;
  hatchObj.Set("reserved", Napi::Number::New(env, reserved));
  BITCODE_BD gradient_angle= hatch->gradient_angle;
  hatchObj.Set("gradient_angle", Napi::Number::New(env, gradient_angle));
  BITCODE_BD gradient_shift= hatch->gradient_shift;
  hatchObj.Set("gradient_shift", Napi::Number::New(env, gradient_shift));
  BITCODE_BL single_color_gradient= hatch->single_color_gradient;
  hatchObj.Set("single_color_gradient", Napi::Number::New(env, single_color_gradient));
  BITCODE_BD gradient_tint= hatch->gradient_tint;
  hatchObj.Set("gradient_tint", Napi::Number::New(env, gradient_tint));
  BITCODE_BL num_colors= hatch->num_colors;
  hatchObj.Set("num_colors", Napi::Number::New(env, num_colors));
  // Dwg_HATCH_Color* colors= hatch->colors;
  BITCODE_T gradient_name= hatch->gradient_name; 
  std::string str_gradient_name = parseChar(gradient_name);
  hatchObj.Set("double_flag", Napi::String::New(env, str_gradient_name));
  /* 1: SPHERICAL, 2: HEMISPHERICAL, 3: CURVED, 4: LINEAR, 5: CYLINDER */

  // BITCODE_BD elevation= hatch->elevation;
  // BITCODE_BE extrusion= hatch->extrusion;
  BITCODE_TV name= hatch->name;
  std::string str_name = parseChar(name);
  hatchObj.Set("name", Napi::String::New(env, str_name));
  BITCODE_B is_solid_fill= hatch->is_solid_fill;
  hatchObj.Set("is_solid_fill", is_solid_fill);
  BITCODE_B is_associative= hatch->is_associative;
  hatchObj.Set("is_associative", is_associative);
  BITCODE_BL num_paths= hatch->num_paths;
  hatchObj.Set("num_paths", Napi::Number::New(env, num_paths));
  Napi::Array pathArray = Napi::Array::New(env);
  for(BITCODE_BL i = 0; i <num_paths; i++){
    Napi::Object pathObject = Napi::Object::New(env);
    Dwg_HATCH_Path *paths= &hatch->paths[i]; 
    pathObject.Set("flag", Napi::Number::New(env, paths->flag));
    // std::cout << "DWG hatch->flag" << i <<" : " << paths->flag << std::endl;
    // std::cout << "DWG hatch->num_segs_or_paths" << i <<" : " << paths->num_segs_or_paths << std::endl;
    // std::cout << "DWG hatch->bulges_present" << i <<" : " << paths->bulges_present << std::endl;
    // std::cout << "DWG hatch->closed" << i <<" : " << paths->closed << std::endl;
    Napi::Array segArray = Napi::Array::New(env);
    Napi::Array lineArray = Napi::Array::New(env);
    Napi::Array arcArray = Napi::Array::New(env);
    for(BITCODE_BL j = 0; j < paths->num_segs_or_paths; j++){
      Napi::Object segObject = Napi::Object::New(env);
      Dwg_HATCH_PathSeg *segs = &paths->segs[j];
      Dwg_HATCH_PolylinePath *polyline_paths = &paths->polyline_paths[j];
      uint32_t lineLength = lineArray.Length();
      if(polyline_paths != nullptr) {
        Napi::Array polyline_paths_point = Napi::Array::New(env);
        // std::cout << "DWG polyline_paths->point" << j <<" : " << polyline_paths->point.x;
        // std::cout << ", " << polyline_paths->point.y << std::endl;
        // std::cout << "DWG polyline_paths bulge" << j <<" : " << polyline_paths->bulge << std::endl;
        polyline_paths_point.Set(zero, Napi::Number::New(env, polyline_paths->point.x));
        polyline_paths_point.Set(1, Napi::Number::New(env, polyline_paths->point.y));
        polyline_paths_point.Set(2, Napi::Number::New(env, 0));

        lineArray.Set(lineLength + zero, Napi::Number::New(env, polyline_paths->point.x));
        lineArray.Set(lineLength + 1, Napi::Number::New(env, polyline_paths->point.y));
        lineArray.Set(lineLength + 2, Napi::Number::New(env, 0));
        segObject.Set("polyline_paths", polyline_paths_point);
      }
      if(segs != nullptr){
        // std::cout << "DWG segs: " << segs << std::endl;
        // std::cout << "DWG segs->curve_type: " << segs->curve_type << std::endl;
        segObject.Set("curve_type", Napi::Number::New(env, segs->curve_type));
        Napi::Array first_endpoint = Napi::Array::New(env, 2);
        first_endpoint.Set(zero, Napi::Number::New(env, segs->first_endpoint.x));
        first_endpoint.Set(1, Napi::Number::New(env, segs->first_endpoint.y));
        segObject.Set("first_endpoint", first_endpoint);
        // std::cout << "DWG segs->first_endpoint: " << segs->first_endpoint.x;
        // std::cout << ", " << segs->first_endpoint.y << std::endl;
        Napi::Array second_endpoint = Napi::Array::New(env, 2);
        second_endpoint.Set(zero, Napi::Number::New(env, segs->second_endpoint.x));
        second_endpoint.Set(1, Napi::Number::New(env, segs->second_endpoint.y));
        segObject.Set("second_endpoint", second_endpoint);
        // std::cout << "DWG segs->second_endpoint: " << segs->second_endpoint.x;
        // std::cout << ", " << segs->second_endpoint.y << std::endl;
        Napi::Array center = Napi::Array::New(env, 2);
        center.Set(zero, Napi::Number::New(env, segs->center.x));
        center.Set(1, Napi::Number::New(env, segs->center.y));
        segObject.Set("center", center);
        // std::cout << "DWG segs->center: " << segs->center.x;
        // std::cout << ", " << segs->center.y << std::endl;
        // std::cout << "DWG segs->radius: " << segs->radius << std::endl;
        segObject.Set("radius", Napi::Number::New(env, segs->radius));
        // std::cout << "DWG segs->start_angle: " << segs->start_angle << std::endl;
        segObject.Set("start_angle", Napi::Number::New(env, segs->start_angle));
        // std::cout << "DWG segs->end_angle: " << segs->end_angle << std::endl;
        segObject.Set("end_angle", Napi::Number::New(env, segs->end_angle));
        // std::cout << "DWG segs->endpoint: " << segs->endpoint.x;
        // std::cout << ", " << segs->endpoint.y << std::endl;
        Napi::Array endpoint = Napi::Array::New(env, 2);
        endpoint.Set(zero, Napi::Number::New(env, segs->endpoint.x));
        endpoint.Set(1, Napi::Number::New(env, segs->endpoint.y));
        segObject.Set("endpoint", endpoint);
        // std::cout << "DWG segs->degree: " << segs->degree << std::endl;
        segObject.Set("end_angle", Napi::Number::New(env, segs->degree));
        // std::cout << "DWG segs->is_rational: " << segs->is_rational << std::endl;
        segObject.Set("end_angle", Napi::Number::New(env, segs->is_rational));
        // std::cout << "DWG segs->is_periodic: " << segs->is_periodic << std::endl;
        segObject.Set("end_angle", Napi::Number::New(env, segs->is_periodic));
        // std::cout << "DWG segs->num_knots: " << segs->num_knots << std::endl;
        Napi::Array knots = Napi::Array::New(env, segs->num_knots);
        BITCODE_BD* knot = segs->knots;
        for(BITCODE_BL k = 0; k < segs->num_knots; k++) {
          // std::cout << "DWG segs->knot: " << knot << ", " << *knot << ", " << &knot << std::endl;
          knots.Set(k, Napi::Number::New(env, *knot));
          knot++;
        }
        segObject.Set("knots", knots);
        // std::cout << "DWG segs->num_control_points: " << segs->num_control_points << std::endl;
        BITCODE_BL num_fitpts = segs->num_fitpts;
        // std::cout << "DWG segs->num_fitpts: " << segs->num_fitpts << std::endl;
        Napi::Array fitptsArr = Napi::Array::New(env, segs->num_fitpts);
        BITCODE_2RD* fitpts = segs->fitpts;
        for(BITCODE_BL k = 0; k < num_fitpts; k++) {
          BITCODE_2RD fitpt = fitpts[k];
          Napi::Array fitptRd = Napi::Array::New(env, 3);
          fitptRd.Set(zero, fitpt.x);
          fitptRd.Set(1, fitpt.y);
          fitptRd.Set(2, 0);
          fitptsArr.Set(k, fitptRd);
        }
        segObject.Set("fitpts", fitptsArr);
        // std::cout << "DWG segs->start_tangent: " << segs->start_tangent.x;
        // std::cout << ", " << segs->start_tangent.y << std::endl;
        Napi::Array start_tangent = Napi::Array::New(env, 2);
        start_tangent.Set(zero, Napi::Number::New(env, segs->start_tangent.x));
        start_tangent.Set(1, Napi::Number::New(env, segs->start_tangent.y));
        segObject.Set("start_tangent", start_tangent);
        // std::cout << "DWG segs->end_tangent: " << segs->end_tangent.x;
        // std::cout << ", " << segs->end_tangent.y << std::endl;
        Napi::Array end_tangent = Napi::Array::New(env, 2);
        end_tangent.Set(zero, Napi::Number::New(env, segs->end_tangent.x));
        end_tangent.Set(1, Napi::Number::New(env, segs->end_tangent.y));
        segObject.Set("end_tangent", end_tangent);
        // BITCODE_BL num_boundary_handles;
        // std::cout << "DWG segs->num_boundary_handles: " << num_boundary_handles << std::endl;
        // BITCODE_H* boundary_handles;
        // BITCODE_B bulges_present;
        // std::cout << "DWG segs->bulges_present: " << bulges_present << std::endl;
        // BITCODE_B closed;
        // std::cout << "DWG segs->closed: " << closed << std::endl;
        // Dwg_HATCH_PolylinePath* polyline_paths;
        // while(true) {
        //   if(polyline_paths == nullptr){
        //     break;
        //   }
        //   // std::cout << "DWG segs->polyline_paths: " << polyline_paths->point.x;
        //   // std::cout << ", " << polyline_paths->point.y << std::endl;
        //   polyline_paths++;
        // }
        if(segs->curve_type == 1) {
          lineArray.Set(lineLength + zero, Napi::Number::New(env, segs->first_endpoint.x));
          lineArray.Set(lineLength + 1, Napi::Number::New(env, segs->first_endpoint.y));
          lineArray.Set(lineLength + 2, Napi::Number::New(env, 0));
        } else if(segs->curve_type == 2) {
          uint32_t arcLength = arcArray.Length();
          Napi::Object arcObject = Napi::Object::New(env);
          arcObject.Set("center", center);
          arcObject.Set("radius", Napi::Number::New(env, segs->radius));
          arcObject.Set("start_angle", Napi::Number::New(env, segs->start_angle));
          arcObject.Set("end_angle", Napi::Number::New(env, segs->end_angle));
          arcArray.Set(arcLength, arcObject);
        }
      } else {
        // std::cout << "seg is null" << std::endl;
      }
      segArray.Set(j, segObject);
    }
    pathObject.Set("seg", segArray);
    pathObject.Set("line", lineArray);
    pathObject.Set("arc", arcArray);
    pathArray.Set(i, pathObject);
  }
  hatchObj.Set("path", pathArray);
  // also named loop
  BITCODE_BS style= hatch->style;
  hatchObj.Set("style", Napi::Number::New(env, style));
  BITCODE_BS pattern_type= hatch->pattern_type;
  hatchObj.Set("pattern_type", Napi::Number::New(env, pattern_type));
  BITCODE_BD angle= hatch->angle;
  hatchObj.Set("angle", Napi::Number::New(env, angle));
  BITCODE_BD scale_spacing= hatch->scale_spacing;
  hatchObj.Set("scale_spacing", Napi::Number::New(env, scale_spacing));
  BITCODE_B double_flag= hatch->double_flag;
  hatchObj.Set("double_flag", Napi::Number::New(env, double_flag));
  BITCODE_BS num_deflines= hatch->num_deflines;
  hatchObj.Set("num_deflines", Napi::Number::New(env, num_deflines));
  Napi::Array deflinesArray = Napi::Array::New(env);
  Dwg_HATCH_DefLine * deflines= hatch->deflines;
  for(BITCODE_BS i =0; i < hatch->num_deflines; i++){
    Napi::Object deflinesObject = Napi::Object::New(env);
    // std::cout << "DWG deflines->angle: " << deflines->angle << std::endl;
    deflinesObject.Set("angle", Napi::Number::New(env,deflines->angle));
    // std::cout << "DWG deflines->pt0: " << deflines->pt0.x;
    // std::cout << ", " << deflines->pt0.y << std::endl;
    Napi::Array pt0 = Napi::Array::New(env, 2);
    pt0.Set(zero, deflines->pt0.x);
    pt0.Set(1, deflines->pt0.y);
    deflinesObject.Set("pt0", pt0);
    // std::cout << "DWG deflines->offset: " << deflines->offset.x;
    // std::cout << ", " << deflines->offset.y << std::endl;
    Napi::Array offset = Napi::Array::New(env, 2);
    offset.Set(zero, deflines->offset.x);
    offset.Set(1, deflines->offset.y);
    deflinesObject.Set("offset", offset);
    // std::cout << "DWG deflines->num_dashes: " << deflines->num_dashes << std::endl;
    BITCODE_BD* dashes = deflines->dashes;
    Napi::Array dashesArray = Napi::Array::New(env, 2);
    for(BITCODE_BS j = 0; j < deflines->num_dashes; j++) {
      // std::cout << "DWG dashes: " << dashes << ", " << *dashes << ", " << &dashes << std::endl;
      dashesArray.Set(j, Napi::Number::New(env, *dashes));
      dashes++;
    }
    deflinesObject.Set("dashes", dashesArray);
    deflinesArray.Set(i, deflinesObject);
    deflines++;
  }
  hatchObj.Set("deflines", deflinesArray);
  // BITCODE_B has_derived= hatch->has_derived;
  // BITCODE_BD pixel_size= hatch->pixel_size;
  // BITCODE_BL num_seeds= hatch->num_seeds;
  // hatchObj.Set("num_seeds", Napi::Number::New(env, num_seeds));
  // BITCODE_2RD * seeds= hatch->seeds;
  // for(BITCODE_BL i = 0; i < num_seeds; i++){
    // std::cout << "DWG seed "<< i << " : " << seeds->x;  
    // std::cout << "," << seeds->y << std::endl;  
  // }
  // std::cout << "DWG hatch->is_gradient_fill: " << is_gradient_fill << std::endl;
  // std::cout << "DWG hatch->reserved: " << reserved << std::endl;
  // std::cout << "DWG hatch->gradient_angle: " << gradient_angle << std::endl;
  // std::cout << "DWG hatch->gradient_shift: " << gradient_shift << std::endl;
  // std::cout << "DWG hatch->single_color_gradient: " << single_color_gradient << std::endl;
  // std::cout << "DWG hatch->gradient_tint: " << gradient_tint << std::endl;
  // std::cout << "DWG hatch->num_colors: " << num_colors << std::endl;
  // std::cout << "DWG hatch->gradient_name: " << gradient_name << std::endl;
  // std::cout << "DWG hatch->elevation: " << elevation << std::endl;
  // std::cout << "DWG hatch->extrusion: " << extrusion.x;
  // std::cout << ", " << extrusion.y;
  // std::cout << ", " << extrusion.z << std::endl;
  // std::cout << "DWG hatch->name: " << parseChar(name) << std::endl;
  // std::cout << "DWG hatch->is_solid_fill: " << is_solid_fill << std::endl;
  // std::cout << "DWG hatch->is_associative: " << is_associative << std::endl;
  // std::cout << "DWG hatch->num_paths: " << num_paths << std::endl;
  // std::cout << "DWG hatch->style: " << style << std::endl;
  // std::cout << "DWG hatch->pattern_type: " << pattern_type << std::endl;
  // std::cout << "DWG hatch->angle: " << angle << std::endl;
  // std::cout << "DWG hatch->scale_spacing: " << scale_spacing << std::endl;
  // std::cout << "DWG hatch->double_flag: " << double_flag << std::endl;
  // std::cout << "DWG hatch->num_deflines: " << num_deflines << std::endl;
  // std::cout << "DWG hatch->has_derived: " << has_derived << std::endl;
  // std::cout << "DWG hatch->pixel_size: " << pixel_size << std::endl;
  // std::cout << "DWG hatch->num_seeds: " << num_seeds << std::endl;
  
  uint32_t length = hatchArray.Length();
  hatchArray.Set(length, hatchObj);
  jsonObj.Set("hatch", hatchArray);
}

// void parseEntityHatch(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
//   Dwg_Entity_HATCH* hatch = object->tio.entity->tio.HATCH;
//   Napi::Array hatchArray = checkObject(jsonObj, "hatch", env);
//   char *layer_name = EntityLayerName(object);
  
//   // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
//   if(!contains(charArray, layer_name)) {
//     return;
//   }

//   Napi::Object hatchObj = Napi::Object::New(env);
//   BITCODE_BL is_gradient_fill = hatch->is_gradient_fill;
//   hatchObj.Set("is_gradient_fill", Napi::Number::New(env, is_gradient_fill));
//   BITCODE_BL reserved= hatch->reserved;
//   hatchObj.Set("reserved", Napi::Number::New(env, reserved));
//   BITCODE_BD gradient_angle= hatch->gradient_angle;
//   hatchObj.Set("gradient_angle", Napi::Number::New(env, gradient_angle));
//   BITCODE_BD gradient_shift= hatch->gradient_shift;
//   hatchObj.Set("gradient_shift", Napi::Number::New(env, gradient_shift));
//   BITCODE_BL single_color_gradient= hatch->single_color_gradient;
//   hatchObj.Set("single_color_gradient", Napi::Number::New(env, single_color_gradient));
//   BITCODE_BD gradient_tint= hatch->gradient_tint;
//   hatchObj.Set("gradient_tint", Napi::Number::New(env, gradient_tint));
//   BITCODE_BL num_colors= hatch->num_colors;
//   hatchObj.Set("num_colors", Napi::Number::New(env, num_colors));
//   // Dwg_HATCH_Color* colors= hatch->colors;
//   BITCODE_T gradient_name= hatch->gradient_name; 
//   std::string str_gradient_name = parseChar(gradient_name);
//   hatchObj.Set("double_flag", Napi::String::New(env, str_gradient_name));
//   /* 1: SPHERICAL, 2: HEMISPHERICAL, 3: CURVED, 4: LINEAR, 5: CYLINDER */

//   // BITCODE_BD elevation= hatch->elevation;
//   // BITCODE_BE extrusion= hatch->extrusion;
//   BITCODE_TV name= hatch->name;
//   std::string str_name = parseChar(name);
//   hatchObj.Set("name", Napi::String::New(env, str_name));
//   BITCODE_B is_solid_fill= hatch->is_solid_fill;
//   hatchObj.Set("is_solid_fill", is_solid_fill);
//   BITCODE_B is_associative= hatch->is_associative;
//   hatchObj.Set("is_associative", is_associative);
//   BITCODE_BL num_paths= hatch->num_paths;
//   hatchObj.Set("num_paths", Napi::Number::New(env, num_paths));
//   Napi::Array pathArray = Napi::Array::New(env);
//   for(BITCODE_BL i = 0; i <num_paths; i++){
//     Napi::Object pathObject = Napi::Object::New(env);
//     Dwg_HATCH_Path *paths= &hatch->paths[i]; 
//     pathObject.Set("flag", Napi::Number::New(env, paths->flag));
//     // std::cout << "DWG hatch->flag" << i <<" : " << paths->flag << std::endl;
//     // std::cout << "DWG hatch->num_segs_or_paths" << i <<" : " << paths->num_segs_or_paths << std::endl;
//     // std::cout << "DWG hatch->bulges_present" << i <<" : " << paths->bulges_present << std::endl;
//     // std::cout << "DWG hatch->closed" << i <<" : " << paths->closed << std::endl;
//     Napi::Array segArray = Napi::Array::New(env);
//     for(BITCODE_BL j = 0; j < paths->num_segs_or_paths; j++){
//       Napi::Object segObject = Napi::Object::New(env);
//       Dwg_HATCH_PathSeg *segs = &paths->segs[j];
//       Dwg_HATCH_PolylinePath *polyline_paths = &paths->polyline_paths[j];
//       if(polyline_paths != nullptr) {
//         Napi::Array polyline_paths_point = Napi::Array::New(env);
//         // std::cout << "DWG polyline_paths->point" << j <<" : " << polyline_paths->point.x;
//         // std::cout << ", " << polyline_paths->point.y << std::endl;
//         // std::cout << "DWG polyline_paths bulge" << j <<" : " << polyline_paths->bulge << std::endl;
//         polyline_paths_point.Set(zero, Napi::Number::New(env, polyline_paths->point.x));
//         polyline_paths_point.Set(1, Napi::Number::New(env, polyline_paths->point.y));
//         polyline_paths_point.Set(2, Napi::Number::New(env, 0));
//         segObject.Set("polyline_paths", polyline_paths_point);
//       }
//       if(segs != nullptr){
//         // std::cout << "DWG segs: " << segs << std::endl;
//         // std::cout << "DWG segs->curve_type: " << segs->curve_type << std::endl;
//         segObject.Set("curve_type", Napi::Number::New(env, segs->curve_type));
//         Napi::Array first_endpoint = Napi::Array::New(env, 2);
//         first_endpoint.Set(zero, Napi::Number::New(env, segs->first_endpoint.x));
//         first_endpoint.Set(1, Napi::Number::New(env, segs->first_endpoint.y));
//         segObject.Set("first_endpoint", first_endpoint);
//         // std::cout << "DWG segs->first_endpoint: " << segs->first_endpoint.x;
//         // std::cout << ", " << segs->first_endpoint.y << std::endl;
//         Napi::Array second_endpoint = Napi::Array::New(env, 2);
//         second_endpoint.Set(zero, Napi::Number::New(env, segs->second_endpoint.x));
//         second_endpoint.Set(1, Napi::Number::New(env, segs->second_endpoint.y));
//         segObject.Set("second_endpoint", second_endpoint);
//         // std::cout << "DWG segs->second_endpoint: " << segs->second_endpoint.x;
//         // std::cout << ", " << segs->second_endpoint.y << std::endl;
//         Napi::Array center = Napi::Array::New(env, 2);
//         center.Set(zero, Napi::Number::New(env, segs->center.x));
//         center.Set(1, Napi::Number::New(env, segs->center.y));
//         segObject.Set("center", center);
//         // std::cout << "DWG segs->center: " << segs->center.x;
//         // std::cout << ", " << segs->center.y << std::endl;
//         // std::cout << "DWG segs->radius: " << segs->radius << std::endl;
//         segObject.Set("radius", Napi::Number::New(env, segs->radius));
//         // std::cout << "DWG segs->start_angle: " << segs->start_angle << std::endl;
//         segObject.Set("start_angle", Napi::Number::New(env, segs->start_angle));
//         // std::cout << "DWG segs->end_angle: " << segs->end_angle << std::endl;
//         segObject.Set("end_angle", Napi::Number::New(env, segs->end_angle));
//         // std::cout << "DWG segs->endpoint: " << segs->endpoint.x;
//         // std::cout << ", " << segs->endpoint.y << std::endl;
//         Napi::Array endpoint = Napi::Array::New(env, 2);
//         endpoint.Set(zero, Napi::Number::New(env, segs->endpoint.x));
//         endpoint.Set(1, Napi::Number::New(env, segs->endpoint.y));
//         segObject.Set("endpoint", endpoint);
//         // std::cout << "DWG segs->degree: " << segs->degree << std::endl;
//         segObject.Set("end_angle", Napi::Number::New(env, segs->degree));
//         // std::cout << "DWG segs->is_rational: " << segs->is_rational << std::endl;
//         segObject.Set("end_angle", Napi::Number::New(env, segs->is_rational));
//         // std::cout << "DWG segs->is_periodic: " << segs->is_periodic << std::endl;
//         segObject.Set("end_angle", Napi::Number::New(env, segs->is_periodic));
//         // std::cout << "DWG segs->num_knots: " << segs->num_knots << std::endl;
//         Napi::Array knots = Napi::Array::New(env, segs->num_knots);
//         BITCODE_BD* knot = segs->knots;
//         for(BITCODE_BL k = 0; k < segs->num_knots; k++) {
//           // std::cout << "DWG segs->knot: " << knot << ", " << *knot << ", " << &knot << std::endl;
//           knots.Set(k, Napi::Number::New(env, *knot));
//           knot++;
//         }
//         segObject.Set("knots", knots);
//         // std::cout << "DWG segs->num_control_points: " << segs->num_control_points << std::endl;
//         BITCODE_BL num_fitpts = segs->num_fitpts;
//         // std::cout << "DWG segs->num_fitpts: " << segs->num_fitpts << std::endl;
//         Napi::Array fitptsArr = Napi::Array::New(env, segs->num_fitpts);
//         BITCODE_2RD* fitpts = segs->fitpts;
//         for(BITCODE_BL k = 0; k < num_fitpts; k++) {
//           BITCODE_2RD fitpt = fitpts[k];
//           Napi::Array fitptRd = Napi::Array::New(env, 3);
//           fitptRd.Set(zero, fitpt.x);
//           fitptRd.Set(1, fitpt.y);
//           fitptRd.Set(2, 0);
//           fitptsArr.Set(k, fitptRd);
//         }
//         segObject.Set("fitpts", fitptsArr);
//         // std::cout << "DWG segs->start_tangent: " << segs->start_tangent.x;
//         // std::cout << ", " << segs->start_tangent.y << std::endl;
//         Napi::Array start_tangent = Napi::Array::New(env, 2);
//         start_tangent.Set(zero, Napi::Number::New(env, segs->start_tangent.x));
//         start_tangent.Set(1, Napi::Number::New(env, segs->start_tangent.y));
//         segObject.Set("start_tangent", start_tangent);
//         // std::cout << "DWG segs->end_tangent: " << segs->end_tangent.x;
//         // std::cout << ", " << segs->end_tangent.y << std::endl;
//         Napi::Array end_tangent = Napi::Array::New(env, 2);
//         end_tangent.Set(zero, Napi::Number::New(env, segs->end_tangent.x));
//         end_tangent.Set(1, Napi::Number::New(env, segs->end_tangent.y));
//         segObject.Set("end_tangent", end_tangent);
//         // BITCODE_BL num_boundary_handles;
//         // std::cout << "DWG segs->num_boundary_handles: " << num_boundary_handles << std::endl;
//         // BITCODE_H* boundary_handles;
//         // BITCODE_B bulges_present;
//         // std::cout << "DWG segs->bulges_present: " << bulges_present << std::endl;
//         // BITCODE_B closed;
//         // std::cout << "DWG segs->closed: " << closed << std::endl;
//         // Dwg_HATCH_PolylinePath* polyline_paths;
//         // while(true) {
//         //   if(polyline_paths == nullptr){
//         //     break;
//         //   }
//         //   // std::cout << "DWG segs->polyline_paths: " << polyline_paths->point.x;
//         //   // std::cout << ", " << polyline_paths->point.y << std::endl;
//         //   polyline_paths++;
//         // }
//       } else {
//         // std::cout << "seg is null" << std::endl;
//       }
//       segArray.Set(j, segObject);
//     }
//     pathObject.Set("seg", segArray);
//     pathArray.Set(i, pathObject);
//   }
//   hatchObj.Set("path", pathArray);
//   // also named loop
//   BITCODE_BS style= hatch->style;
//   hatchObj.Set("style", Napi::Number::New(env, style));
//   BITCODE_BS pattern_type= hatch->pattern_type;
//   hatchObj.Set("pattern_type", Napi::Number::New(env, pattern_type));
//   BITCODE_BD angle= hatch->angle;
//   hatchObj.Set("angle", Napi::Number::New(env, angle));
//   BITCODE_BD scale_spacing= hatch->scale_spacing;
//   hatchObj.Set("scale_spacing", Napi::Number::New(env, scale_spacing));
//   BITCODE_B double_flag= hatch->double_flag;
//   hatchObj.Set("double_flag", Napi::Number::New(env, double_flag));
//   BITCODE_BS num_deflines= hatch->num_deflines;
//   hatchObj.Set("num_deflines", Napi::Number::New(env, num_deflines));
//   Napi::Array deflinesArray = Napi::Array::New(env);
//   Dwg_HATCH_DefLine * deflines= hatch->deflines;
//   for(BITCODE_BS i =0; i < hatch->num_deflines; i++){
//     Napi::Object deflinesObject = Napi::Object::New(env);
//     // std::cout << "DWG deflines->angle: " << deflines->angle << std::endl;
//     deflinesObject.Set("angle", Napi::Number::New(env,deflines->angle));
//     // std::cout << "DWG deflines->pt0: " << deflines->pt0.x;
//     // std::cout << ", " << deflines->pt0.y << std::endl;
//     Napi::Array pt0 = Napi::Array::New(env, 2);
//     pt0.Set(zero, deflines->pt0.x);
//     pt0.Set(1, deflines->pt0.y);
//     deflinesObject.Set("pt0", pt0);
//     // std::cout << "DWG deflines->offset: " << deflines->offset.x;
//     // std::cout << ", " << deflines->offset.y << std::endl;
//     Napi::Array offset = Napi::Array::New(env, 2);
//     offset.Set(zero, deflines->offset.x);
//     offset.Set(1, deflines->offset.y);
//     deflinesObject.Set("offset", offset);
//     // std::cout << "DWG deflines->num_dashes: " << deflines->num_dashes << std::endl;
//     BITCODE_BD* dashes = deflines->dashes;
//     Napi::Array dashesArray = Napi::Array::New(env, 2);
//     for(BITCODE_BS j = 0; j < deflines->num_dashes; j++) {
//       // std::cout << "DWG dashes: " << dashes << ", " << *dashes << ", " << &dashes << std::endl;
//       dashesArray.Set(j, Napi::Number::New(env, *dashes));
//       dashes++;
//     }
//     deflinesObject.Set("dashes", dashesArray);
//     deflinesArray.Set(i, deflinesObject);
//     deflines++;
//   }
//   hatchObj.Set("deflines", deflinesArray);
//   // BITCODE_B has_derived= hatch->has_derived;
//   // BITCODE_BD pixel_size= hatch->pixel_size;
//   // BITCODE_BL num_seeds= hatch->num_seeds;
//   // hatchObj.Set("num_seeds", Napi::Number::New(env, num_seeds));
//   // BITCODE_2RD * seeds= hatch->seeds;
//   // for(BITCODE_BL i = 0; i < num_seeds; i++){
//     // std::cout << "DWG seed "<< i << " : " << seeds->x;  
//     // std::cout << "," << seeds->y << std::endl;  
//   // }
//   // std::cout << "DWG hatch->is_gradient_fill: " << is_gradient_fill << std::endl;
//   // std::cout << "DWG hatch->reserved: " << reserved << std::endl;
//   // std::cout << "DWG hatch->gradient_angle: " << gradient_angle << std::endl;
//   // std::cout << "DWG hatch->gradient_shift: " << gradient_shift << std::endl;
//   // std::cout << "DWG hatch->single_color_gradient: " << single_color_gradient << std::endl;
//   // std::cout << "DWG hatch->gradient_tint: " << gradient_tint << std::endl;
//   // std::cout << "DWG hatch->num_colors: " << num_colors << std::endl;
//   // std::cout << "DWG hatch->gradient_name: " << gradient_name << std::endl;
//   // std::cout << "DWG hatch->elevation: " << elevation << std::endl;
//   // std::cout << "DWG hatch->extrusion: " << extrusion.x;
//   // std::cout << ", " << extrusion.y;
//   // std::cout << ", " << extrusion.z << std::endl;
//   // std::cout << "DWG hatch->name: " << parseChar(name) << std::endl;
//   // std::cout << "DWG hatch->is_solid_fill: " << is_solid_fill << std::endl;
//   // std::cout << "DWG hatch->is_associative: " << is_associative << std::endl;
//   // std::cout << "DWG hatch->num_paths: " << num_paths << std::endl;
//   // std::cout << "DWG hatch->style: " << style << std::endl;
//   // std::cout << "DWG hatch->pattern_type: " << pattern_type << std::endl;
//   // std::cout << "DWG hatch->angle: " << angle << std::endl;
//   // std::cout << "DWG hatch->scale_spacing: " << scale_spacing << std::endl;
//   // std::cout << "DWG hatch->double_flag: " << double_flag << std::endl;
//   // std::cout << "DWG hatch->num_deflines: " << num_deflines << std::endl;
//   // std::cout << "DWG hatch->has_derived: " << has_derived << std::endl;
//   // std::cout << "DWG hatch->pixel_size: " << pixel_size << std::endl;
//   // std::cout << "DWG hatch->num_seeds: " << num_seeds << std::endl;
  
//   uint32_t length = hatchArray.Length();
//   hatchArray.Set(length, hatchObj);
//   jsonObj.Set("hatch", hatchArray);
// }

void parseEntitySolid(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_SOLID* solid = object->tio.entity->tio.SOLID;
  char *layer_name = EntityLayerName(object);
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }
  
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array solidArray = checkObject(jsonObj, "solid", env);
  
  Napi::Object solidObj = Napi::Object::New(env);
  Napi::Array corner1Array = Napi::Array::New(env, 3);
  corner1Array.Set(zero, Napi::Number::New(env, solid->corner1.x));
  corner1Array.Set(1, Napi::Number::New(env, solid->corner1.y));
  corner1Array.Set(2, Napi::Number::New(env, 0));

  Napi::Array corner2Array = Napi::Array::New(env, 3);
  corner2Array.Set(zero, Napi::Number::New(env, solid->corner2.x));
  corner2Array.Set(1, Napi::Number::New(env, solid->corner2.y));
  corner2Array.Set(2, Napi::Number::New(env, 0));

  Napi::Array corner3Array = Napi::Array::New(env, 3);
  corner3Array.Set(zero, Napi::Number::New(env, solid->corner3.x));
  corner3Array.Set(1, Napi::Number::New(env, solid->corner3.y));
  corner3Array.Set(2, Napi::Number::New(env, 0));

  Napi::Array corner4Array = Napi::Array::New(env, 3);
  corner4Array.Set(zero, Napi::Number::New(env, solid->corner4.x));
  corner4Array.Set(1, Napi::Number::New(env, solid->corner4.y));
  corner4Array.Set(2, Napi::Number::New(env, 0));

  solidObj.Set("corner1", corner1Array);
  solidObj.Set("corner2", corner2Array);
  solidObj.Set("corner3", corner3Array);
  solidObj.Set("corner4", corner4Array);

  uint32_t length = solidArray.Length();
  solidArray.Set(length, solidObj);
  jsonObj.Set("solid", solidArray);
}

void parseEntityARC(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_ARC* arc = object->tio.entity->tio.ARC;
  
  char *layer_name = EntityLayerName(object);
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }
  
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array arcArray = checkObject(jsonObj, "arc", env);
  
  Napi::Object arcObj = Napi::Object::New(env);
  Napi::Array centerArray = Napi::Array::New(env, 3);
  centerArray.Set(zero, Napi::Number::New(env, arc->center.x));
  centerArray.Set(1, Napi::Number::New(env, arc->center.y));
  centerArray.Set(2, Napi::Number::New(env, arc->center.z));
  
  arcObj.Set("center", centerArray);
  arcObj.Set("radius", Napi::Number::New(env, arc->radius));
  arcObj.Set("start_angle", Napi::Number::New(env, arc->start_angle));
  arcObj.Set("end_angle", Napi::Number::New(env, arc->end_angle));
  uint32_t length = arcArray.Length();
  arcArray.Set(length, arcObj);
  jsonObj.Set("arc", arcArray);
}

// M�ؽ�Ʈ
// void parseEntityMText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
//   Dwg_Entity_MTEXT* mtext = object->tio.entity->tioMTEXT;
//   Napi::Array mtextArray = checkObject(jsonObj, "mtext");
  
//   Napi::Object mtextObj = Napi::Object::New(env);
//   Napi::Array centerArray = Napi::Array::New(env, 3);
//   centerArray.Set(zero, Napi::Number::New(env, mtext->ins_pt.x));
//   centerArray.Set(1, Napi::Number::New(env, mtext->ins_pt.y));
//   centerArray.Set(2, Napi::Number::New(env, mtext->ins_pt.z));
  
//   mtextObj.Set("center", centerArray);
  
//   char* text = mtext->text;
//   std::string strText = parseChar(text);
//   // std::cout << "DWG mtext->text: " << strText << std::endl;
//   mtextObj.Set("text", Napi::String::New(env, strText));
//   uint32_t length = mtextArray.Length();
//   mtextArray.Set(length, mtextObj);
//   jsonObj.Set("mtext", mtextArray);
// }

// �ؽ�Ʈ
// void parseEntityText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
//   Dwg_Entity_TEXT* text = object->tio.entity->tioTEXT;
//   Napi::Array textArray = checkObject(jsonObj, "text");
  
//   Napi::Object textObj = Napi::Object::New(env);
//   Napi::Array centerArray = Napi::Array::New(env, 3);
//   centerArray.Set(zero, Napi::Number::New(env, text->ins_pt.x));
//   centerArray.Set(1, Napi::Number::New(env, text->ins_pt.y));
//   centerArray.Set(2, Napi::Number::New(env, 0));
//   BITCODE_RD rotation = text->rotation;
//   textObj.Set("rotation", rotation);
//   textObj.Set("center", centerArray);
//   std::string strText = "";
//   if(text->text_value != nullptr){
//     std::cout << "DWG text->text: " << text->text_value << std::endl;
//     std::string strText = parseChar(text->text_value);
//   }
//   // std::cout << "DWG text->text: " << strText << std::endl;
//   textObj.Set("text", Napi::String::New(env, strText));
//   uint32_t length = textArray.Length();
//   textArray.Set(length, textObj);
//   jsonObj.Set("text", textArray);
// }

// �
void parseEntitySpline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_SPLINE* spline = object->tio.entity->tio.SPLINE;
  char *layer_name = EntityLayerName(object);
  
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array splineArray = checkObject(jsonObj, "spline", env);
  
  Napi::Object splineObj = Napi::Object::New(env);
  Napi::Array points = Napi::Array::New(env);
  BITCODE_3DPOINT* fit_pts = spline->fit_pts;
  for(BITCODE_BS i = 0; i < spline->num_fit_pts; i++){
    Napi::Array point = Napi::Array::New(env);
    point.Set(zero, Napi::Number::New(env, fit_pts->x));
    point.Set(1, Napi::Number::New(env, fit_pts->y));
    point.Set(2, Napi::Number::New(env, fit_pts->z));
    points.Set(i, point);
    fit_pts++; // �����͸� ���� ��ҷ� �̵�
  }

  splineObj.Set("points", points);
  uint32_t length = splineArray.Length();
  splineArray.Set(length, splineObj);
  jsonObj.Set("spline", splineArray);
}

// X��
void parseEntityXline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_XLINE* xline = object->tio.entity->tio.XLINE;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }


  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array xlineArray = checkObject(jsonObj, "xline", env);
  
  Napi::Object xlineObj = Napi::Object::New(env);
  Napi::Array point = Napi::Array::New(env);
  Napi::Array vector = Napi::Array::New(env);
  point.Set(zero, Napi::Number::New(env, xline->point.x));
  point.Set(1, Napi::Number::New(env, xline->point.y));
  point.Set(2, Napi::Number::New(env, xline->point.z));
  vector.Set(zero, Napi::Number::New(env, xline->vector.x));
  vector.Set(1, Napi::Number::New(env, xline->vector.y));
  vector.Set(2, Napi::Number::New(env, xline->vector.z));
  xlineObj.Set("point", point);
  xlineObj.Set("vector", vector);
  uint32_t length = xlineArray.Length();
  xlineArray.Set(length, xlineObj);
  jsonObj.Set("xline", xlineArray);
}

// ����
void parseEntityRay(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_RAY* ray = object->tio.entity->tio.RAY;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array rayArray = checkObject(jsonObj, "ray", env);
  
  Napi::Object rayObj = Napi::Object::New(env);
  Napi::Array point = Napi::Array::New(env);
  Napi::Array vector = Napi::Array::New(env);
  point.Set(zero, Napi::Number::New(env, ray->point.x));
  point.Set(1, Napi::Number::New(env, ray->point.y));
  point.Set(2, Napi::Number::New(env, ray->point.z));
  vector.Set(zero, Napi::Number::New(env, ray->vector.x));
  vector.Set(1, Napi::Number::New(env, ray->vector.y));
  vector.Set(2, Napi::Number::New(env, ray->vector.z));
  
  rayObj.Set("point", point);
  rayObj.Set("vector", vector);
  uint32_t length = rayArray.Length();
  rayArray.Set(length, rayObj);
  jsonObj.Set("ray", rayArray);
}

//��
void parseEntityPoint(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_POINT* point = object->tio.entity->tio.POINT;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array pointArray = checkObject(jsonObj, "point", env);
  
  Napi::Object pointObj = Napi::Object::New(env);
  Napi::Array pointArr = Napi::Array::New(env);
  pointArr.Set(zero, Napi::Number::New(env, point->x));
  pointArr.Set(1, Napi::Number::New(env, point->y));
  pointArr.Set(2, Napi::Number::New(env, point->z));
  
  pointObj.Set("point", pointArr);
  uint32_t length = pointArray.Length();
  pointArray.Set(length, pointObj);
  jsonObj.Set("point", pointArray);
}

// void parseLayout(Dwg_Object_Object* object, Napi::Array jsonArr, Napi::Env env) {
//   Dwg_Object_LAYOUT* layout = object->tio.LAYOUT;

//   // std::cout << "DWG layout.layout_name: " << layout->layout_name << std::endl;
//   // std::cout << "DWG layout.INSBASE.x: " << layout->INSBASE.x << std::endl;
//   // std::cout << "DWG layout.INSBASE.y: " << layout->INSBASE.y << std::endl;
//   // std::cout << "DWG layout.INSBASE.z: " << layout->INSBASE.z << std::endl;

//   // std::cout << "DWG layout.LIMMIN.x: " << layout->LIMMIN.x << std::endl;
//   // std::cout << "DWG layout.LIMMIN.y: " << layout->LIMMIN.y << std::endl;
  
//   // std::cout << "DWG layout.LIMMAX.x: " << layout->LIMMAX.x << std::endl;
//   // std::cout << "DWG layout.LIMMAX.y: " << layout->LIMMAX.y << std::endl;
  
//   // std::cout << "DWG layout.UCSORG.x: " << layout->UCSORG.x << std::endl;
//   // std::cout << "DWG layout.UCSORG.y: " << layout->UCSORG.y << std::endl;
//   // std::cout << "DWG layout.UCSORG.z: " << layout->UCSORG.z << std::endl;

//   // std::cout << "DWG layout.UCSXDIR.x: " << layout->UCSXDIR.x << std::endl;
//   // std::cout << "DWG layout.UCSXDIR.y: " << layout->UCSXDIR.y << std::endl;
//   // std::cout << "DWG layout.UCSXDIR.z: " << layout->UCSXDIR.z << std::endl;

//   // std::cout << "DWG layout.UCSYDIR.x: " << layout->UCSYDIR.x << std::endl;
//   // std::cout << "DWG layout.UCSYDIR.y: " << layout->UCSYDIR.y << std::endl;
//   // std::cout << "DWG layout.UCSYDIR.z: " << layout->UCSYDIR.z << std::endl;

//   // std::cout << "DWG layout.EXTMIN.x: " << layout->EXTMIN.x << std::endl;
//   // std::cout << "DWG layout.EXTMIN.y: " << layout->EXTMIN.y << std::endl;
//   // std::cout << "DWG layout.EXTMIN.z: " << layout->EXTMIN.z << std::endl;

//   // std::cout << "DWG layout.EXTMAX.x: " << layout->EXTMAX.x << std::endl;
//   // std::cout << "DWG layout.EXTMAX.y: " << layout->EXTMAX.y << std::endl;
//   // std::cout << "DWG layout.EXTMAX.z: " << layout->EXTMAX.z << std::endl;
//   // std::cout << "DWG layout.num_viewports: " << layout->num_viewports << std::endl;
//   // std::cout << "DWG layout.active_viewport: " << layout->active_viewport << std::endl;
//   // std::cout << "DWG layout.viewports: " << layout->viewports << std::endl;

//   Dwg_Object_LAYER_INDEX* layout_index = object->tio.LAYER_INDEX;
//   // std::cout << "DWG layout.num_entries: " << layout_index->num_entries << std::endl;
//   Dwg_Object_PLOTSETTINGS* plotSetting = object->tio.PLOTSETTINGS;
//   // std::cout << "DWG plotSetting.paperWidth: " << plotSetting->paper_width << std::endl;
//   // std::cout << "DWG plotSetting.paperHeight: " << plotSetting->paper_height << std::endl;

// }

void parseBlock(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_BLOCK* block = object->tio.entity->tio.BLOCK;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);


  BITCODE_2RD base_pt = block->base_pt;
  Napi::Array blockArray = checkObject(jsonObj, "block", env);
  Napi::Object blockObject = Napi::Object::New(env);
  Napi::Array block_pt = Napi::Array::New(env, 3);
  block_pt.Set(zero, Napi::Number::New(env, base_pt.x));
  block_pt.Set(1, Napi::Number::New(env, base_pt.y));
  block_pt.Set(2, Napi::Number::New(env, 0));
  uint32_t length = blockArray.Length();
  blockObject.Set("point", block_pt);
  std::string name = parseChar(block->name);
  blockObject.Set("name", Napi::String::New(env, name));
  blockArray.Set(length, blockObject);
  jsonObj.Set("block", blockArray);
}

void parseBlockHeader(Dwg_Object_Object* object_object, Napi::Object jsonObj, Napi::Env env) {
  Dwg_Object_BLOCK_HEADER* blockHeader = object_object->tio.BLOCK_HEADER;
  Napi::Array blockHeaderArray = checkObject(jsonObj, "blockHeader", env);
  Napi::Object blockHeaderObject = Napi::Object::New(env);
  std::string name = parseChar(blockHeader->name);
  blockHeaderObject.Set("name", Napi::String::New(env, name));
  // std::cout << "blockHeader->name: " << name << std::endl;
  // BITCODE_BL __iterator = blockHeader->__iterator;
  // // std::cout << "blockHeader->__iterator: " << __iterator << std::endl;
  // BITCODE_B anonymous = blockHeader->anonymous;
  // // std::cout << "blockHeader->anonymous: " << anonymous << std::endl;
  // BITCODE_B hasattrs = blockHeader->hasattrs;
  // // std::cout << "blockHeader->hasattrs: " << hasattrs << std::endl;
  // BITCODE_B blkisxref = blockHeader->blkisxref;
  // // std::cout << "blockHeader->blkisxref: " << blkisxref << std::endl;
  // BITCODE_B xrefoverlaid = blockHeader->xrefoverlaid;
  // // std::cout << "blockHeader->xrefoverlaid: " << xrefoverlaid << std::endl;
  // BITCODE_B loaded_bit = blockHeader->loaded_bit;
  // // std::cout << "blockHeader->loaded_bit: " << loaded_bit << std::endl;
  BITCODE_BL num_owned = blockHeader->num_owned;
  // std::cout << "blockHeader->num_owned: " << num_owned << std::endl;
  // BITCODE_3DPOINT base_pt = blockHeader->base_pt;
  // // std::cout << "blockHeader->base_pt: " << base_pt.x;
  // // std::cout << ", " << base_pt.y;
  // // std::cout << ", " << base_pt.z << std::endl;
  // BITCODE_TV xref_pname = blockHeader->xref_pname;
  // std::string str_xref_pname = parseChar(xref_pname);
  // // std::cout << "blockHeader->xref_pname: " << str_xref_pname << std::endl;
  // BITCODE_RL num_inserts = blockHeader->num_inserts;
  // // std::cout << "blockHeader->num_inserts: " << num_inserts << std::endl;
  // BITCODE_TV description = blockHeader->description;
  // std::string str_description = parseChar(description);
  // // std::cout << "blockHeader->description: " << str_description << std::endl;
  // BITCODE_BL preview_size = blockHeader->preview_size;
  // // std::cout << "blockHeader->preview_size: " << preview_size << std::endl;
  // BITCODE_TF preview = blockHeader->preview;
  // std::string str_preview = parseChar(preview);
  // // std::cout << "blockHeader->preview: " << str_preview << std::endl;
  // BITCODE_BS insert_units = blockHeader->insert_units;
  // // std::cout << "blockHeader->insert_units: " << insert_units << std::endl;
  // BITCODE_B explodable = blockHeader->explodable;
  // // std::cout << "blockHeader->explodable: " << explodable << std::endl;
  // BITCODE_RC block_scaling = blockHeader->block_scaling;
  // std::cout << "blockHeader->block_scaling: " << block_scaling << std::endl;
  BITCODE_H block_entity = blockHeader->block_entity;
  Napi::Array blockHeaderComponent = Napi::Array::New(env);
  if(block_entity != nullptr){
    // std::cout << "blockHeader->block_entity start"<< std::endl;
    parseDWGObject(block_entity->obj, blockHeaderComponent, env);
    // std::cout << "blockHeader->block_entity end"<< std::endl;
  }
  BITCODE_H first_entity = blockHeader->first_entity;
  if(first_entity != nullptr){
    // std::cout << "blockHeader->first_entity start"<< std::endl;
    parseDWGObject(first_entity->obj, blockHeaderComponent, env);
    // std::cout << "blockHeader->first_entity end"<< std::endl;
  }
  BITCODE_H last_entity = blockHeader->last_entity;
  if(last_entity != nullptr){
    // std::cout << "blockHeader->last_entity start"<< std::endl;
    parseDWGObject(last_entity->obj, blockHeaderComponent, env);
    // std::cout << "blockHeader->last_entity end"<< std::endl;
  }
  BITCODE_H* entities = blockHeader->entities;
  // std::cout << "blockHeader->entities start"<< std::endl;
  for(BITCODE_BL i = 0; i < num_owned; i++) {
    Dwg_Object* object = entities[i]->obj;
    parseDWGObject(object, blockHeaderComponent, env);
  }
  // std::cout << "blockHeader->entities end"<< std::endl;
  BITCODE_H endblk_entity = blockHeader->endblk_entity;
  // std::cout << "blockHeader->endblk_entity start"<< std::endl;
  parseDWGObject(endblk_entity->obj, blockHeaderComponent, env);
  // std::cout << "blockHeader->endblk_entity end"<< std::endl;

  // std::cout << "blockHeader->inserts end"<< std::endl;
  BITCODE_H layout = blockHeader->layout;
  // std::cout << "blockHeader->layout start"<< std::endl;
  parseDWGObject(layout->obj, blockHeaderComponent, env);
  // std::cout << "blockHeader->layout end"<< std::endl;
  // BITCODE_RS flag2 = blockHeader->flag2;
  // // std::cout << "blockHeader->flag2: " << flag2 << std::endl;
  // BITCODE_RC unknown_r11 = blockHeader->unknown_r11;
  // // std::cout << "blockHeader->unknown_r11: " << unknown_r11 << std::endl;
  // BITCODE_RL block_offset_r11 = blockHeader->block_offset_r11;
  // // std::cout << "blockHeader->block_offset_r11: " << block_offset_r11 << std::endl;
  blockHeaderObject.Set("component", blockHeaderComponent);
  uint32_t length = blockHeaderArray.Length();
  blockHeaderArray.Set(length, blockHeaderObject);
  jsonObj.Set("blockHeader", blockHeaderArray);
}

void parseEntityInsert(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_INSERT* insert = object->tio.entity->tio.INSERT;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);

  Napi::Array insertArray = checkObject(jsonObj, "insert", env);
  Napi::Object insertObject = Napi::Object::New(env);
  // Napi::Object insertObject = createObject(env);
  Napi::Array baseArray = Napi::Array::New(env, 3);
  BITCODE_3DPOINT   ins_pt = insert->ins_pt;
  baseArray.Set(zero, Napi::Number::New(env, ins_pt.x));
  baseArray.Set(1, Napi::Number::New(env, ins_pt.y));
  baseArray.Set(2, Napi::Number::New(env, ins_pt.z));
  insertObject.Set("base", baseArray);
  // std::cout << "insert->ins_pt: " << ins_pt.x;
  // std::cout << ", " << ins_pt.y;
  // std::cout << ", " << ins_pt.z << std::endl;
  // BITCODE_BB        scale_flag = insert->scale_flag;
  // std::cout << "insert->scale_flag: " << scale_flag << std::endl;
  Napi::Array scaleArray = Napi::Array::New(env, 3);
  BITCODE_3DPOINT   scale = insert->scale;
  scaleArray.Set(zero, Napi::Number::New(env, scale.x));
  scaleArray.Set(1, Napi::Number::New(env, scale.y));
  scaleArray.Set(2, Napi::Number::New(env, scale.z));
  insertObject.Set("scale", scaleArray);
  // std::cout << "insert->scale: " << scale.x;
  // std::cout << ", " << scale.y;
  // std::cout << ", " << scale.z << std::endl;
  BITCODE_BD        rotation = insert->rotation;
  insertObject.Set("rotation", Napi::Number::New(env, rotation));
  // std::cout << "insert->rotation: " << rotation << std::endl;
  // BITCODE_BE        extrusion = insert->extrusion;
  // // std::cout << "insert->extrusion: " << extrusion.x;
  // // std::cout << ", " << extrusion.y;
  // // std::cout << ", " << extrusion.z << std::endl;
  // BITCODE_B         has_attribs = insert->has_attribs;
  // // std::cout << "insert->has_attribs: " << has_attribs << std::endl;
  // BITCODE_BL        num_owned = insert->num_owned;
  // // std::cout << "insert->num_owned: " << num_owned << std::endl;

  BITCODE_H   block_header = insert->block_header;
  if(block_header != nullptr){
    parseBlockHeader(block_header->obj->tio.object, insertObject, env);
  }

  // BITCODE_RS  num_cols = insert->num_cols;
  // // std::cout << "insert->num_cols: " << num_cols << std::endl;
  // BITCODE_RS  num_rows = insert->num_rows;
  // // std::cout << "insert->num_rows: " << num_rows << std::endl;
  // BITCODE_RD  col_spacing = insert->col_spacing;
  // // std::cout << "insert->col_spacing: " << col_spacing << std::endl;
  // BITCODE_RD  row_spacing = insert->row_spacing;
  // // std::cout << "insert->row_spacing: " << row_spacing << std::endl;
  BITCODE_TV  block_name = insert->block_name;
  if(block_name != nullptr){
    std::string str_block_name = parseChar(block_name);
    // std::cout << "insert->block_name: " << str_block_name << std::endl;
  }
  uint32_t length = insertArray.Length();
  insertArray.Set(length, insertObject);
  jsonObj.Set("insert", insertArray);
}

// ��
void parseEntityLine(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_LINE* line = object->tio.entity->tio.LINE;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);

  Dwg_Bitcode_3BD start = line->start;
  Dwg_Bitcode_3BD end = line->end;
  Napi::Array lineArray = checkObject(jsonObj, "line", env);
  
  Napi::Object lineObj = Napi::Object::New(env);
  Napi::Array startArray = Napi::Array::New(env, 3);
  Napi::Array endArray = Napi::Array::New(env, 3);
  startArray.Set(zero, Napi::Number::New(env, start.x));
  startArray.Set(1, Napi::Number::New(env, start.y));
  startArray.Set(2, Napi::Number::New(env, start.z));

  // 'end' �迭�� �� ����
  endArray.Set(zero, Napi::Number::New(env, end.x));
  endArray.Set(1, Napi::Number::New(env, end.y));
  endArray.Set(2, Napi::Number::New(env, end.z));

  // 'lineObj' ��ü�� 'start'�� 'end' �迭�� ����
  lineObj.Set("start", startArray);
  lineObj.Set("end", endArray);
  uint32_t length = lineArray.Length();

  Napi::Array newLineArray = Napi::Array::New(env, length + 1);
  for (uint32_t i = 0; i < length; ++i) {
    newLineArray.Set(i, lineArray.Get(i));
  }
  newLineArray.Set(length, lineObj);
  jsonObj.Set("line", newLineArray);
}

// ��
void parseEntityCircle(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_CIRCLE* circle = object->tio.entity->tio.CIRCLE;
  char *layer_name = EntityLayerName(object);
  // std::cout << "DWG layerName: " << layer_name <<","<<checkLayerName<< std::endl;
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);

  Napi::Array circleArray = checkObject(jsonObj, "circle", env);
  
  Napi::Object circleObj = Napi::Object::New(env);
  Napi::Array centerArray = Napi::Array::New(env, 3);
  centerArray.Set(zero, Napi::Number::New(env, circle->center.x));
  centerArray.Set(1, Napi::Number::New(env, circle->center.y));
  centerArray.Set(2, Napi::Number::New(env, circle->center.z));
  
  circleObj.Set("center", centerArray);
  circleObj.Set("radius", Napi::Number::New(env, circle->radius));
  uint32_t length = circleArray.Length();
  circleArray.Set(length, circleObj);
  jsonObj.Set("circle", circleArray);
}

// ������
void parseEntityLwPolyline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_LWPOLYLINE* lwPolyline = object->tio.entity->tio.LWPOLYLINE;
  char *layer_name = EntityLayerName(object);
  
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);

  Napi::Array lwPolylineArray = checkObject(jsonObj,"lwPolyline", env);
  
  Napi::Object pointObj = Napi::Object::New(env);
  Napi::Array pointArray = Napi::Array::New(env);

  Dwg_Bitcode_2RD* points = lwPolyline->points;
  for (BITCODE_BL i = 0; i < lwPolyline->num_points; ++i) {
    Napi::Array point = Napi::Array::New(env);
    point.Set(zero, Napi::Number::New(env, points->x));
    point.Set(1, Napi::Number::New(env, points->y));
    point.Set(2, Napi::Number::New(env, 0));
    pointArray.Set(i, point);
    points++; // �����͸� ���� ��ҷ� �̵�
  }
  pointObj.Set("point", pointArray);
  // BITCODE_BL num_widths = lwPolyline->num_widths;
  // std::cout << "DWG lwpolyline.num_widths: " << num_widths << std::endl;;
  // Dwg_LWPOLYLINE_width* widths = lwPolyline->widths;
  // for(BITCODE_BL i = 0; i < num_widths; i++){
  //   // Dwg_LWPOLYLINE_width width = widths[i];
  //   // Napi::Array array = Napi::Array::New(env);
  //   // std::cout << "DWG lwpolyline.widths "<< i << " : " << width.start;
  //   // std::cout << ", " << width.end << std::endl;;
  // }
  uint32_t length = lwPolylineArray.Length();
  lwPolylineArray.Set(length, pointObj);
  jsonObj.Set("lwPolyline", lwPolylineArray);
}

void parseEntityPolyline2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_POLYLINE_2D* polyline2D = object->tio.entity->tio.POLYLINE_2D;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);

  Napi::Array polyline2DArray = checkObject(jsonObj, "polyline2D", env);
  Napi::Object polyline2DObject = Napi::Object::New(env);
  Napi::Array polyline2DComponent = Napi::Array::New(env);
  // Napi::Object polyline2DObject = createObject(env);
  // BITCODE_B has_vertex = polyline2D->has_vertex;
  // // std::cout << "polyline2D->has_vertex: " << has_vertex << std::endl;
  BITCODE_BL num_owned = polyline2D->num_owned;
  // std::cout << "polyline2D->num_owned: " << num_owned << std::endl;
  BITCODE_H first_vertex = polyline2D->first_vertex;
  if(first_vertex != nullptr){
    // std::cout << "polyline2D->first_vertex start"<< std::endl;
    parseDWGObject(first_vertex->obj, polyline2DComponent, env);
    // std::cout << "polyline2D->first_vertex end"<< std::endl;
  }
  BITCODE_H last_vertex = polyline2D->last_vertex;
  if(last_vertex != nullptr){
    // std::cout << "polyline2D->last_vertex start"<< std::endl;
    parseDWGObject(last_vertex->obj, polyline2DComponent, env);
    // std::cout << "polyline2D->last_vertex end"<< std::endl;
  }
  BITCODE_H *vertex = polyline2D->vertex;
  for(BITCODE_BL i = 0; i < num_owned; i++) {
    Dwg_Object* object = vertex[i]->obj;
    parseDWGObject(object, polyline2DComponent, env);
  }
  BITCODE_H seqend = polyline2D->seqend;
  if(seqend != nullptr){
    // std::cout << "polyline2D->seqend start"<< std::endl;
    parseDWGObject(seqend->obj, polyline2DComponent, env);
    // std::cout << "polyline2D->seqend end"<< std::endl;
  }
  /* 1: closed, 2: curve_fit, 4: spline_fit, 8: 3d, 0x10: 3dmesh,
  0x20: mesh_closed_in_n, 0x40: polyface_mesh, 0x80: ltype_continuous */
  // BITCODE_BS flag = polyline2D->flag;
  // // std::cout << "polyline2D->flag: " << flag << std::endl;
  // BITCODE_BS curve_type = polyline2D->curve_type;
  // // std::cout << "polyline2D->curve_type: " << curve_type << std::endl;
  // BITCODE_BD start_width = polyline2D->start_width;
  // // std::cout << "polyline2D->start_width: " << start_width << std::endl;
  // BITCODE_BD end_width = polyline2D->end_width;
  // // std::cout << "polyline2D->end_width: " << end_width << std::endl;
  // BITCODE_BT thickness = polyline2D->thickness;
  // // std::cout << "polyline2D->thickness: " << thickness << std::endl;
  // BITCODE_BD elevation = polyline2D->elevation;
  // // std::cout << "polyline2D->elevation: " << elevation << std::endl;
  // BITCODE_BE extrusion = polyline2D->extrusion;
  // // std::cout << "polyline2D->extrusion: " << extrusion.x;
  // // std::cout << ", " << extrusion.y;
  // // std::cout << ", " << extrusion.z << std::endl;
  // BITCODE_BL extra_r11_size = polyline2D->extra_r11_size;
  // std::cout << "polyline2D->extra_r11_size: " << extra_r11_size << std::endl;
  BITCODE_TV extra_r11_text = polyline2D->extra_r11_text;
  if(extra_r11_text != nullptr){
    std::string str_extra_r11_text = parseChar(extra_r11_text);
    // std::cout << "polyline2D->extra_r11_text: " << str_extra_r11_text << std::endl;
  }
  // BITCODE_BS num_m_verts = polyline2D->num_m_verts; // sometime used in preR13, to delete
  // // std::cout << "polyline2D->num_m_verts: " << num_m_verts << std::endl;
  // BITCODE_BS num_n_verts = polyline2D->num_n_verts; // sometime used in preR13, to delete
  // // std::cout << "polyline2D->num_n_verts: " << num_n_verts << std::endl;
  // std::cout << "DWG length: " << polyline2DArray.Length() << std::endl;
  polyline2DObject.Set("component", polyline2DComponent);
  uint32_t length = polyline2DArray.Length();
  // std::cout << "polyline2D logs 1: " << std::endl;
  polyline2DArray.Set(length, polyline2DObject);
  // std::cout << "polyline2D logs 2: " << std::endl;
  jsonObj.Set("polyline2D", polyline2DArray);
  // std::cout << "polyline2D logs 3: " << std::endl;
}

void parseEntityVertex2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_VERTEX_2D* vertex2D = object->tio.entity->tio.VERTEX_2D;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);

  Napi::Array vertex2DArray = checkObject(jsonObj, "vertex2D", env);
  Napi::Object vertex2DObject = Napi::Object::New(env);
  // BITCODE_RC flag = vertex2D->flag;
  // // std::cout << "vertex2D->flag: " << flag << std::endl;
  BITCODE_3BD point = vertex2D->point;
  Napi::Array pointArray = Napi::Array::New(env);
  pointArray.Set(zero, Napi::Number::New(env, point.x));
  pointArray.Set(1, Napi::Number::New(env, point.y));
  pointArray.Set(2, Napi::Number::New(env, point.z));
  vertex2DObject.Set("point", pointArray);
  // std::cout << "vertex2D->point: " << point.x;
  // std::cout << ", " << point.y;
  // std::cout << ", " << point.z << std::endl;
  // BITCODE_BD start_width = vertex2D->start_width;
  // vertex2DObject.Set("start_width", Napi::Number::New(env, vertex2D->start_width));
  // std::cout << "vertex2D->start_width: " << start_width << std::endl;
  // BITCODE_BD end_width = vertex2D->end_width;
  // vertex2DObject.Set("end_width", Napi::Number::New(env, vertex2D->end_width));
  // std::cout << "vertex2D->end_width: " << end_width << std::endl;
  // BITCODE_BL id = vertex2D->id; /* R2010+ */
  // // std::cout << "vertex2D->id: " << id << std::endl;
  // BITCODE_BD bulge = vertex2D->bulge;
  // // std::cout << "vertex2D->bulge: " << bulge << std::endl;
  // BITCODE_BD tangent_dir = vertex2D->tangent_dir;
  // // std::cout << "vertex2D->tangent_dir: " << tangent_dir << std::endl;
  uint32_t length = vertex2DArray.Length();
  vertex2DArray.Set(length, vertex2DObject);
  jsonObj.Set("vertex2D", vertex2DArray);
}

// Ÿ��(?) �Ľ�
void parseEntityEllipse(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
  Dwg_Entity_ELLIPSE* ellipse = object->tio.entity->tio.ELLIPSE;
  char *layer_name = EntityLayerName(object);
    
  // if(strcmp(checkLayerName, "") != 0 && strcmp(checkLayerName, layer_name) != 0) {
  if(!contains(charArray, layer_name)) {
    return;
  }
  // Napi::Array ellipseArray = jsonObj.Get("ellipse").As<Napi::Array>();
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array ellipseArray = checkObject(jsonObj, "ellipse", env);
  Napi::Object ellipseObject = Napi::Object::New(env);

  BITCODE_3BD center = ellipse->center;
  Napi::Array centerArray = Napi::Array::New(env);
  centerArray.Set(zero, Napi::Number::New(env, center.x));
  centerArray.Set(1, Napi::Number::New(env, center.y));
  centerArray.Set(2, Napi::Number::New(env, center.z));
  ellipseObject.Set("center", centerArray);
  // std::cout << "ellipse->center: " << center.x;
  // std::cout << ", " << center.y;
  // std::cout << ", " << center.z << std::endl;
  BITCODE_3BD sm_axis = ellipse->sm_axis;
  Napi::Array axisArray = Napi::Array::New(env);
  axisArray.Set(zero, Napi::Number::New(env, sm_axis.x));
  axisArray.Set(1, Napi::Number::New(env, sm_axis.y));
  axisArray.Set(2, Napi::Number::New(env, sm_axis.z));
  ellipseObject.Set("sm_axis", axisArray);
  // std::cout << "ellipse->sm_axis: " << sm_axis.x;
  // std::cout << ", " << sm_axis.y;
  // std::cout << ", " << sm_axis.z << std::endl;
  BITCODE_BE extrusion = ellipse->extrusion;
  Napi::Array extrusionArray = Napi::Array::New(env);
  extrusionArray.Set(zero, Napi::Number::New(env, extrusion.x));
  extrusionArray.Set(1, Napi::Number::New(env, extrusion.y));
  extrusionArray.Set(2, Napi::Number::New(env, extrusion.z));
  ellipseObject.Set("extrusion", extrusionArray);
  // std::cout << "ellipse->extrusion: " << extrusion.x;
  // std::cout << ", " << extrusion.y;
  // std::cout << ", " << extrusion.z << std::endl;
  BITCODE_BD axis_ratio = ellipse->axis_ratio; /* i.e RadiusRatio */
  ellipseObject.Set("axis_ratio", Napi::Number::New(env, axis_ratio));
  // std::cout << "ellipse->axis_ratio: " << axis_ratio << std::endl;
  BITCODE_BD start_angle = ellipse->start_angle;
  ellipseObject.Set("start_angle", Napi::Number::New(env, start_angle));
  // std::cout << "ellipse->start_angle: " << start_angle << std::endl;
  BITCODE_BD end_angle = ellipse->end_angle;
  ellipseObject.Set("end_angle", Napi::Number::New(env, end_angle));
  // std::cout << "ellipse->end_angle: " << end_angle << std::endl;

  uint32_t length = ellipseArray.Length();
  ellipseArray.Set(length, ellipseObject);
  jsonObj.Set("ellipse", ellipseArray);
}
// ����(?) �Ľ�
// void parseEntityAttdef(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env)  {
//   Dwg_Entity_ATTDEF* attdef = object->tio.entity->tioATTDEF;

//   Napi::Array attdefArray = jsonObj.Get("attdef").As<Napi::Array>();
//   Napi::Object attdefObject = Napi::Object::New(env);

//   // BITCODE_BD elevation = attdef->elevation;
//   // // std::cout << "attdef->elevation: " << elevation << std::endl;
//   BITCODE_2DPOINT ins_pt = attdef->ins_pt;
//   Napi::Array insArray = Napi::Array::New(env);
//   insArray.Set(zero, Napi::Number::New(env, ins_pt.x));
//   insArray.Set(1, Napi::Number::New(env, ins_pt.y));
//   insArray.Set(2, Napi::Number::New(env, zero));
//   attdefObject.Set("ins_pt", insArray);
//   // std::cout << "attdef->ins_pt: " << ins_pt.x;
//   // std::cout << ", " << ins_pt.y << std::endl;
//   BITCODE_2DPOINT alignment_pt = attdef->alignment_pt;
//   Napi::Array alignmentArray = Napi::Array::New(env);
//   alignmentArray.Set(zero, Napi::Number::New(env, alignment_pt.x));
//   alignmentArray.Set(1, Napi::Number::New(env, alignment_pt.y));
//   alignmentArray.Set(2, Napi::Number::New(env,zero));
//   attdefObject.Set("alignment_pt", alignmentArray);
//   // std::cout << "attdef->alignment_pt: " << alignment_pt.x;
//   // std::cout << ", " << alignment_pt.y << std::endl;
//   BITCODE_BE extrusion = attdef->extrusion;
//   Napi::Array extrusionArray = Napi::Array::New(env);
//   extrusionArray.Set(zero, Napi::Number::New(env, extrusion.x));
//   extrusionArray.Set(1, Napi::Number::New(env, extrusion.y));
//   extrusionArray.Set(2, Napi::Number::New(env, extrusion.z));
//   attdefObject.Set("extrusion", extrusionArray);
//   // std::cout << "attdef->extrusion: " << extrusion.x;
//   // std::cout << ", " << extrusion.y;
//   // std::cout << ", " << extrusion.z << std::endl;
//   BITCODE_RD thickness = attdef->thickness;
//   attdefObject.Set("thickness", Napi::Number::New(env, thickness));
//   // std::cout << "attdef->thickness: " << thickness << std::endl;
//   BITCODE_RD oblique_angle = attdef->oblique_angle;
//   attdefObject.Set("oblique_angle", Napi::Number::New(env, oblique_angle));
//   // std::cout << "attdef->oblique_angle: " << oblique_angle << std::endl;
//   BITCODE_RD rotation = attdef->rotation;
//   attdefObject.Set("rotation", Napi::Number::New(env, rotation));
//   // std::cout << "attdef->rotation: " << rotation << std::endl;
//   BITCODE_RD height = attdef->height;
//   attdefObject.Set("height", Napi::Number::New(env, height));
//   // std::cout << "attdef->height: " << height << std::endl;
//   BITCODE_RD width_factor = attdef->width_factor;
//   attdefObject.Set("width_factor", Napi::Number::New(env, width_factor));
//   // std::cout << "attdef->width_factor: " << width_factor << std::endl;
//   BITCODE_T default_value = attdef->default_value;
//   std::string str_default_value = parseChar(default_value);
//   attdefObject.Set("default_value", Napi::String::New(env, str_default_value));
//   // std::cout << "attdef->default_value: " << default_value << std::endl;
//   BITCODE_BS generation = attdef->generation;
//   attdefObject.Set("generation", Napi::Number::New(env, generation));
//   // std::cout << "attdef->generation: " << generation << std::endl;
//   BITCODE_BS horiz_alignment = attdef->horiz_alignment;
//   attdefObject.Set("horiz_alignment", Napi::Number::New(env, horiz_alignment));
//   // std::cout << "attdef->horiz_alignment: " << horiz_alignment << std::endl;
//   BITCODE_BS vert_alignment = attdef->vert_alignment;
//   attdefObject.Set("vert_alignment", Napi::Number::New(env, vert_alignment));
//   // std::cout << "attdef->vert_alignment: " << vert_alignment << std::endl;
//   BITCODE_RC dataflags = attdef->dataflags;
//   attdefObject.Set("dataflags", dataflags);
//   // std::cout << "attdef->dataflags: " << dataflags << std::endl;
//   BITCODE_RC class_version = attdef->class_version; /* R2010+ */
//   attdefObject.Set("class_version", class_version);
//   // std::cout << "attdef->class_version: " << class_version << std::endl;
//   BITCODE_RC type = attdef->type;    /* R2018+ */
//   attdefObject.Set("type", type);
//   // std::cout << "attdef->type: " << type << std::endl;
//   BITCODE_T tag = attdef->tag;
//   std::string str_tag = parseChar(tag);
//   attdefObject.Set("tag", Napi::String::New(env, str_tag));
//   // std::cout << "attdef->tag: " << str_tag << std::endl;
//   BITCODE_BS field_length = attdef->field_length;
//   attdefObject.Set("field_length", Napi::Number::New(env, field_length));
//   // std::cout << "attdef->field_length: " << field_length << std::endl;
//   BITCODE_RC flags = attdef->flags; /* => HEADER.AFLAGS */
//   attdefObject.Set("flags", flags);
//   // std::cout << "attdef->flag: " << flags << std::endl;
//   BITCODE_B lock_position_flag = attdef->lock_position_flag;
//   attdefObject.Set("lock_position_flag", lock_position_flag);
//   // std::cout << "attdef->lock_position_flag: " << lock_position_flag << std::endl;
//   // BITCODE_H style = attdef->style;
//   // BITCODE_H mtext_style = attdef->mtext_style; /* R2018+ TODO */
//   BITCODE_BS annotative_data_size = attdef->annotative_data_size; /* R2018+ */
//   attdefObject.Set("annotative_data_size", Napi::Number::New(env, annotative_data_size));
//   // std::cout << "attdef->annotative_data_size: " << annotative_data_size << std::endl;
//   BITCODE_RC annotative_data_bytes = attdef->annotative_data_bytes;
//   attdefObject.Set("annotative_data_bytes", annotative_data_bytes);
//   // std::cout << "attdef->annotative_data_bytes: " << annotative_data_bytes << std::endl;
//   // BITCODE_H  annotative_app = attdef->annotative_app;
//   BITCODE_BS annotative_short = attdef->annotative_short;
//   attdefObject.Set("annotative_short", Napi::Number::New(env, annotative_short));
//   // std::cout << "attdef->annotative_short: " << annotative_short << std::endl;
//   BITCODE_RC attdef_class_version = attdef->attdef_class_version; /* R2010+ */
//   attdefObject.Set("attdef_class_version", attdef_class_version);
//   // std::cout << "attdef->attdef_class_version: " << attdef_class_version << std::endl;
//   BITCODE_T prompt = attdef->prompt;
//   std::string str_prompt = parseChar(prompt);
//   attdefObject.Set("prompt", Napi::String::New(env, str_prompt));
//   // std::cout << "attdef->prompt: " << str_prompt << std::endl;

//   uint32_t length = attdefArray.Length();
//   attdefArray.Set(length, attdefObject);
//   jsonObj.Set("attdef", attdefArray);
// }
// DIMENSION_LINEAR
// void parseEntityDimensionLinear(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env)  {
//   Dwg_Entity_DIMENSION_LINEAR* dimensionLinear = object->tio.entity->tioDIMENSION_LINEAR;

//   Napi::Array dimensionLinearArray = jsonObj.Get("dimensionLinear").As<Napi::Array>();
//   Napi::Object dimensionLinearObject = Napi::Object::New(env);

//   // BITCODE_RC class_version =  dimensionLinear->class_version;
//   // // std::cout << "dimensionLinear->class_version: " << class_version << std::endl;
//   BITCODE_BE extrusion =  dimensionLinear->extrusion;
//   Napi::Array extrusionArray = Napi::Array::New(env);
//   extrusionArray.Set(zero, Napi::Number::New(env, extrusion.x));
//   extrusionArray.Set(1, Napi::Number::New(env, extrusion.y));
//   extrusionArray.Set(2, Napi::Number::New(env, extrusion.z));
//   dimensionLinearObject.Set("extrusion", extrusionArray);
//   // std::cout << "dimensionLinear->extrusion: " << extrusion.x;
//   // std::cout << ", " << extrusion.y;
//   // std::cout << ", " << extrusion.z << std::endl;
//   BITCODE_3BD def_pt =  dimensionLinear->def_pt;
//   Napi::Array defArray = Napi::Array::New(env);
//   defArray.Set(zero, Napi::Number::New(env, def_pt.x));
//   defArray.Set(1, Napi::Number::New(env, def_pt.y));
//   defArray.Set(2, Napi::Number::New(env, def_pt.z));
//   dimensionLinearObject.Set("def_pt", defArray);
//   // std::cout << "dimensionLinear->def_pt: " << def_pt.x;
//   // std::cout << ", " << def_pt.y;
//   // std::cout << ", " << def_pt.z << std::endl;
//   BITCODE_2RD text_midpt =  dimensionLinear->text_midpt;
//   Napi::Array text_midptArray = Napi::Array::New(env);
//   text_midptArray.Set(zero, Napi::Number::New(env, text_midpt.x));
//   text_midptArray.Set(1, Napi::Number::New(env, text_midpt.y));
//   text_midptArray.Set(2, Napi::Number::New(env, 0));
//   dimensionLinearObject.Set("text_midpt", text_midptArray);
//   // std::cout << "dimensionLinear->text_midpt: " << text_midpt.x;
//   // std::cout << ", " << text_midpt.y;
//   BITCODE_BD elevation =  dimensionLinear->elevation;
//   dimensionLinearObject.Set("elevation", Napi::Number::New(env, elevation));
//   // std::cout << "dimensionLinear->elevation: " << elevation << std::endl;
//   BITCODE_RC flag =  dimensionLinear->flag;
//   dimensionLinearObject.Set("flag", flag);
//   // std::cout << "dimensionLinear->flag: " << flag << std::endl;
//   BITCODE_RC flag1 =  dimensionLinear->flag1;
//   dimensionLinearObject.Set("flag1", flag1);
//   // std::cout << "dimensionLinear->flag1: " << flag1 << std::endl;
//   BITCODE_T user_text =  dimensionLinear->user_text;
//   std::string str_user_text = parseChar(user_text);
//   dimensionLinearObject.Set("user_text", Napi::String::New(env, str_user_text));
//   // std::cout << "dimensionLinear->user_text: " << str_user_text << std::endl;
//   BITCODE_BD text_rotation =  dimensionLinear->text_rotation;
//   dimensionLinearObject.Set("text_rotation", Napi::Number::New(env, text_rotation));
//   // std::cout << "dimensionLinear->text_rotation: " << text_rotation << std::endl;
//   BITCODE_BD horiz_dir =  dimensionLinear->horiz_dir;
//   dimensionLinearObject.Set("horiz_dir", Napi::Number::New(env, horiz_dir));
//   // std::cout << "dimensionLinear->horiz_dir: " << horiz_dir << std::endl;
//   BITCODE_3BD ins_scale =  dimensionLinear->ins_scale;
//   Napi::Array ins_scaleArray = Napi::Array::New(env);
//   ins_scaleArray.Set(zero, Napi::Number::New(env, ins_scale.x));
//   ins_scaleArray.Set(1, Napi::Number::New(env, ins_scale.y));
//   ins_scaleArray.Set(2, Napi::Number::New(env, ins_scale.z));
//   dimensionLinearObject.Set("text_midpt", text_midptArray);
//   // std::cout << "dimensionLinear->ins_scale: " << ins_scale.x;
//   // std::cout << ", " << ins_scale.y;
//   // std::cout << ", " << ins_scale.z << std::endl;
//   BITCODE_BD ins_rotation =  dimensionLinear->ins_rotation;
//   dimensionLinearObject.Set("ins_rotation", Napi::Number::New(env, ins_rotation));
//   // std::cout << "dimensionLinear->ins_rotation: " << ins_rotation << std::endl;
//   BITCODE_BS attachment =  dimensionLinear->attachment;
//   dimensionLinearObject.Set("attachment", Napi::Number::New(env, attachment));
//   // std::cout << "dimensionLinear->attachment: " << attachment << std::endl;
//   BITCODE_BS lspace_style =  dimensionLinear->lspace_style;
//   dimensionLinearObject.Set("lspace_style", Napi::Number::New(env, lspace_style));
//   // std::cout << "dimensionLinear->lspace_style: " << lspace_style << std::endl;
//   BITCODE_BD lspace_factor =  dimensionLinear->lspace_factor;
//   dimensionLinearObject.Set("lspace_factor", Napi::Number::New(env, lspace_factor));
//   // std::cout << "dimensionLinear->lspace_factor: " << lspace_factor << std::endl;
//   BITCODE_BD act_measurement =  dimensionLinear->act_measurement;
//   dimensionLinearObject.Set("act_measurement", Napi::Number::New(env, act_measurement));
//   // std::cout << "dimensionLinear->act_measurement: " << act_measurement << std::endl;
//   BITCODE_B unknown =  dimensionLinear->unknown;
//   dimensionLinearObject.Set("unknown", Napi::Number::New(env, unknown));
//   // std::cout << "dimensionLinear->unknown: " << unknown << std::endl;
//   BITCODE_B flip_arrow1 =  dimensionLinear->flip_arrow1;
//   dimensionLinearObject.Set("flip_arrow1", Napi::Number::New(env, flip_arrow1));
//   // std::cout << "dimensionLinear->flip_arrow1: " << flip_arrow1 << std::endl;
//   BITCODE_B flip_arrow2 =  dimensionLinear->flip_arrow2;
//   dimensionLinearObject.Set("flip_arrow2", Napi::Number::New(env, flip_arrow2));
//   // std::cout << "dimensionLinear->flip_arrow2: " << flip_arrow2 << std::endl;
//   BITCODE_2RD clone_ins_pt =  dimensionLinear->clone_ins_pt;
//   Napi::Array clone_ins_ptArray = Napi::Array::New(env);
//   clone_ins_ptArray.Set(zero, Napi::Number::New(env, clone_ins_pt.x));
//   clone_ins_ptArray.Set(1, Napi::Number::New(env, clone_ins_pt.y));
//   clone_ins_ptArray.Set(2, Napi::Number::New(env, 0));
//   dimensionLinearObject.Set("extrusion", clone_ins_ptArray);
//   // std::cout << "dimensionLinear->clone_ins_pt: " << clone_ins_pt. x;
//   // std::cout << ", " << clone_ins_pt.y << std::endl;
//   // BITCODE_H dimstyle =  dimensionLinear->dimstyle;
//   // BITCODE_H block =  dimensionLinear->block;
//   BITCODE_3BD xline1_pt =  dimensionLinear->xline1_pt;
//   Napi::Array xline1_ptArray = Napi::Array::New(env);
//   xline1_ptArray.Set(zero, Napi::Number::New(env, xline1_pt.x));
//   xline1_ptArray.Set(1, Napi::Number::New(env, xline1_pt.y));
//   xline1_ptArray.Set(2, Napi::Number::New(env, xline1_pt.z));
//   dimensionLinearObject.Set("xline1_pt", xline1_ptArray);
//   // std::cout << "dimensionLinear->xline1_pt: " << xline1_pt. x;
//   // std::cout << ", " << xline1_pt.y;
//   // std::cout << ", " << xline1_pt.z << std::endl;
//   BITCODE_3BD xline2_pt =  dimensionLinear->xline2_pt;
//   Napi::Array xline2_ptArray = Napi::Array::New(env);
//   xline2_ptArray.Set(zero, Napi::Number::New(env, xline2_pt.x));
//   xline2_ptArray.Set(1, Napi::Number::New(env, xline2_pt.y));
//   xline2_ptArray.Set(2, Napi::Number::New(env, xline2_pt.z));
//   dimensionLinearObject.Set("xline2_pt", xline2_ptArray);
//   // std::cout << "dimensionLinear->xline2_pt: " << xline2_pt. x;
//   // std::cout << ", " << xline2_pt.y;
//   // std::cout << ", " << xline2_pt.z << std::endl;
//   BITCODE_BD oblique_angle =  dimensionLinear->oblique_angle;
//   dimensionLinearObject.Set("oblique_angle", Napi::Number::New(env,oblique_angle));
//   // std::cout << "dimensionLinear->oblique_angle: " << oblique_angle << std::endl;
//   BITCODE_BD dim_rotation =  dimensionLinear->dim_rotation;
//   dimensionLinearObject.Set("dim_rotation", Napi::Number::New(env,dim_rotation));
//   // std::cout << "dimensionLinear->dim_rotation: " << dim_rotation << std::endl;

//   uint32_t length = dimensionLinearArray.Length();
//   dimensionLinearArray.Set(length, dimensionLinearObject);
//   jsonObj.Set("dimensionLinear", dimensionLinearArray);
// }
// SOLID
// void parseEntitySolid(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env)  {
//   Dwg_Entity_SOLID* solid = object->tio.entity->tioSOLID;

//   Napi::Array solidArray = jsonObj.Get("solid").As<Napi::Array>();
//   Napi::Object solidObject = Napi::Object::New(env);

//   // BITCODE_BT thickness = solid->thickness;
//   // // std::cout << "solid->thickness: " << thickness << std::endl;
//   // BITCODE_BD elevation = solid->elevation;
//   // // std::cout << " solid->elevation: " << elevation << std::endl;
//   BITCODE_2RD corner1 = solid->corner1;
//   Napi::Array corner1Array = Napi::Array::New(env);
//   corner1Array.Set(zero, Napi::Number::New(env, corner1.x));
//   corner1Array.Set(1, Napi::Number::New(env, corner1.y));
//   corner1Array.Set(2, Napi::Number::New(env, 0));
//   solidObject.Set("corner1", corner1Array);
//   // std::cout << " solid->corner1: " << corner1. x;
//   // std::cout << " , " << corner1.y << std::endl;
//   BITCODE_2RD corner2 = solid->corner2;
//   Napi::Array corner2Array = Napi::Array::New(env);
//   corner2Array.Set(zero, Napi::Number::New(env, corner2.x));
//   corner2Array.Set(1, Napi::Number::New(env, corner2.y));
//   corner2Array.Set(2, Napi::Number::New(env, 0));
//   solidObject.Set("corner2", corner2Array);
//   // std::cout << " solid->corner2: " << corner2. x;
//   // std::cout << " , " << corner2.y << std::endl;
//   BITCODE_2RD corner3 = solid->corner3;
//   Napi::Array corner3Array = Napi::Array::New(env);
//   corner3Array.Set(zero, Napi::Number::New(env, corner3.x));
//   corner3Array.Set(1, Napi::Number::New(env, corner3.y));
//   corner3Array.Set(2, Napi::Number::New(env, 0));
//   solidObject.Set("corner3", corner3Array);
//   // std::cout << " solid->corner3: " << corner3. x;
//   // std::cout << " , " << corner3.y << std::endl;
//   BITCODE_2RD corner4 = solid->corner4;
//   Napi::Array corner4Array = Napi::Array::New(env);
//   corner4Array.Set(zero, Napi::Number::New(env, corner4.x));
//   corner4Array.Set(1, Napi::Number::New(env, corner4.y));
//   corner4Array.Set(2, Napi::Number::New(env, 0));
//   solidObject.Set("corner4", corner4Array);
//   // std::cout << " solid->corner4: " << corner4. x;
//   // std::cout << " , " << corner4.y;
//   BITCODE_BE extrusion = solid->extrusion;
//   Napi::Array extrusionArray = Napi::Array::New(env);
//   extrusionArray.Set(zero, Napi::Number::New(env, extrusion.x));
//   extrusionArray.Set(1, Napi::Number::New(env, extrusion.y));
//   extrusionArray.Set(2, Napi::Number::New(env, extrusion.z));
//   solidObject.Set("extrusion", extrusionArray);
//   // std::cout << "solid->extrusion: " << extrusion.x;
//   // std::cout << ", " << extrusion.y;
//   // std::cout << ", " << extrusion.z << std::endl;

//   uint32_t length = solidArray.Length();
//   solidArray.Set(length, solidObject);
//   jsonObj.Set("solid", solidArray);
// }

std::string ExtractDWGData(Dwg_Data dwg, Napi::Array jsonArr, Napi::Env env) {
  double modelxmin = dwg_model_x_min(&dwg);
  double modelxmax = dwg_model_x_max(&dwg);
  double modelymin = dwg_model_y_min(&dwg);
  double modelymax = dwg_model_y_max(&dwg);
  double modelzmin = dwg_model_z_min(&dwg);
  double modelzmax = dwg_model_z_max(&dwg);
  Napi::Object jsonObj = checkArray(jsonArr, "viewPosition", env);
  Napi::Array viewPosition = checkObject(jsonObj,"viewPosition", env);
  
  viewPosition.Set(zero, Napi::Number::New(env, (modelxmax+modelxmin)/2));
  viewPosition.Set(1, Napi::Number::New(env, (modelymax+modelymin)/2));
  viewPosition.Set(2, Napi::Number::New(env, (modelzmax+modelzmin)/2));
  jsonObj.Set("viewPosition", viewPosition);
  return std::string(dwg_version_type(dwg.header.from_version));
}

void parseDWGObject(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env) {
    if(object){
      int type = (int)object->fixedtype;
      switch(type){
        case DWG_TYPE_ARC:
          parseEntityARC(object, jsonArr, env);
          break;
        case DWG_TYPE_LINE:
          parseEntityLine(object, jsonArr, env);
          break;
        case DWG_TYPE_CIRCLE:
          parseEntityCircle(object, jsonArr, env);
          break;
        case DWG_TYPE_POLYLINE_2D:
          parseEntityPolyline2D(object, jsonArr, env);
          break;
        case DWG_TYPE_POLYLINE_3D:
          // parseEntityLine(object, jsonArr, env);
          break;
        case DWG_TYPE_BLOCK:
          // parseBlock(object, jsonArr, env);
          break;
        case DWG_TYPE_INSERT:
          // parseEntityInsert(object, jsonArr, env);
          break;
        case DWG_TYPE_MLINE:
          // parseEntityMLine(object, jsonArr, env);
          break;
        case DWG_TYPE_ELLIPSE:
          parseEntityEllipse(object, jsonArr, env);
          break;
        case DWG_TYPE_POINT:
          parseEntityPoint(object, jsonArr, env);
          break;
        case DWG_TYPE_HELIX:
          // parseEntityHELIX(object, jsonArr, env);
          break;
        case DWG_TYPE_TEXT:
          // parseEntityText(object, jsonArr, env);
          break;
        case DWG_TYPE_MTEXT:
          // parseEntityMText(object, jsonArr, env);
          break;
        case DWG_TYPE_SPLINE:
          parseEntitySpline(object, jsonArr, env);
          break;
        case DWG_TYPE_XLINE:
          parseEntityXline(object, jsonArr, env);
          break;
        case DWG_TYPE_LWPOLYLINE:
          parseEntityLwPolyline(object, jsonArr, env);
          break;
        case DWG_TYPE_SOLID:
          parseEntitySolid(object, jsonArr, env);
          break;
        case DWG_TYPE_HATCH:
          // parseEntityHatch(object, jsonArr, env);
          break;
      }
    }
    
    
    // if (strcmp(object->name, "LINE") == 0) {
    //   // std::string layerName = EntityLayerName(entity);
      
    // } else if (strcmp(object->name,"CIRCLE") == 0) {
    //   parseEntityCircle(entity, jsonObj, env);
    // }else if (strcmp(object->name, "LWPOLYLINE") == 0){
    //   parseEntityLwPolyline(entity, jsonObj, env);
    // } else if (strcmp(object->name, "BLOCK") == 0){
    //   parseBlock(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "LAYOUT") == 0){
    // //   parseLayout(object_object, jsonObj, env);
    // } else if (strcmp(object->name, "ARC") == 0){
    //   parseEntityARC(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "MTEXT") == 0){
    // //   parseEntityMText(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "TEXT") == 0){
    // //   parseEntityText(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "SPLINE") == 0){
    // //   parseEntitySpline(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "XLINE") == 0){
    // //   parseEntityXline(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "RAY") == 0){
    // //   parseEntityRay(entity, jsonObj, env);
    // } else if (strcmp(object->name, "POINT") == 0){
    //   parseEntityPoint(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "HATCH") == 0){
    // //   parseEntityHatch(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "INSERT") == 0){
    // //   parseEntityInsert(entity, jsonObj, env);
    // } else if (strcmp(object->name, "POLYLINE_2D") == 0){
    //   parseEntityPolyline2D(entity, jsonObj, env);
    // } else if (strcmp(object->name, "VERTEX_2D") == 0){
    //   parseEntityVertex2D(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "ELLIPSE") == 0){
    // //   parseEntityEllipse(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "ATTDEF") == 0){
    // //   parseEntityAttdef(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "DIMENSION_LINEAR") == 0){
    // //   parseEntityDimensionLinear(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "SOLID") == 0){
    // //   parseEntitySolid(entity, jsonObj, env);
    // // } else if (strcmp(object->name, "LAYER") == 0){
    // //   Dwg_Object_LAYER * layer = object_object->tio.LAYER;
    // //   if(layer == nullptr){
    // //     // std::cout << "DWG layer null " << std::endl;  
    // //   }else {
    // //     // std::cout << "DWG layer check start: " <<  std::endl;
    // //     // std::cout << "DWG layer.name: " << parseChar(layer->name) << std::endl;
    // //     // std::cout << "DWG layer.flag: " << layer->flag << std::endl;
    // //     // std::cout << "DWG layer.used: " << layer->used << std::endl;
    // //     // std::cout << "DWG layer.frozen: " << layer->frozen << std::endl;
    // //     // std::cout << "DWG layer.flag0: " << layer->flag0 << std::endl;
    // //     // std::cout << "DWG layer.on: " << layer->on << std::endl;
    // //     // std::cout << "DWG layer.frozen_in_new: " << layer->frozen_in_new << std::endl;
    // //     // std::cout << "DWG layer.locked: " << layer->locked << std::endl;
    // //     // std::cout << "DWG layer.plotflag: " << layer->plotflag << std::endl;
    // //     // std::cout << "DWG layer.linewt: " << layer->linewt << std::endl;
    // //     // std::cout << "DWG layer check end: "  << std::endl;
    // //   }
    // // } else {
    //   // std::cout << "DWG obj not parsing "<< object->name << std::endl;
    // }
}

std::string CheckObjectRef(Dwg_Data *restrict dwg, Napi::Array jsonArr, Napi::Env env) {
  std::cout << "DWG dwg->num_object_refs: " << dwg->num_objects << std::endl;
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++){
    Dwg_Object *object = &dwg->object[i];
    if(object != nullptr){
      // Dwg_Object* object = dwg->object_ref[i]->obj;
      parseDWGObject(object, jsonArr, env);  
    }
    
  }
  return std::string("check end");
}

char* EntityLayerName(Dwg_Object* object) {
  int error;
  char *name;
  Dwg_Object_Entity *ent = object->tio.entity;
  name = dwg_ent_get_layer_name (ent, &error);
  if (!error){
    return name;
  }
  return name;
}

std::string CheckLayer(Dwg_Data *restrict dwg, Napi::Object jsonObj, Napi::Env env) {
  std::cout << "DWG layer: 1" << std::endl;
  unsigned int layer_count = dwg_get_layer_count(dwg);
  std::cout << "DWG layer: 2 "<< layer_count << std::endl;
  Dwg_Object_LAYER ** layer_objects =  dwg_get_layers(dwg);
  std::cout << "DWG layer: 3" << std::endl;
  Napi::Array layerNames = Napi::Array::New(env);
  std::cout << "DWG layer: 4" << std::endl;
  int error;
  char *name;
  for (unsigned int i = 0; i < layer_count; i++)
    {
      std::cout << "DWG layer: 5" << std::endl;
      std::cout << "DWG layer.index: " << i << std::endl;
      Dwg_Object_LAYER * layer = layer_objects[i];
      // std::cout << "DWG index: " << i << std::endl;
      if(layer){
        // std::cout << "DWG layer check start index: " << i << std::endl;
        name = dwg_obj_layer_get_name(layer, &error);
        uint32_t length = layerNames.Length();
        layerNames.Set(length, Napi::String::New(env, name));
        std::cout << "DWG layer.name: " << name << std::endl;
        free(name);
        
        // std::cout << "DWG layer.flag: " << layer->flag << std::endl;
        // std::cout << "DWG layer.used: " << layer->used << std::endl;
        // std::cout << "DWG layer.frozen: " << layer->frozen << std::endl;
        // std::cout << "DWG layer.flag0: " << layer->flag0 << std::endl;
        // std::cout << "DWG layer.on: " << layer->on << std::endl;
        // std::cout << "DWG layer.frozen_in_new: " << layer->frozen_in_new << std::endl;
        // std::cout << "DWG layer.locked: " << layer->locked << std::endl;
        // std::cout << "DWG layer.plotflag: " << layer->plotflag << std::endl;
        // std::cout << "DWG layer.linewt: " << layer->linewt << std::endl;
        // std::cout << "DWG layer check end index: " << i << std::endl;
      }
    }
  jsonObj.Set("layerNames", layerNames);
  return std::string("check end");
}

// DWG ������ �Ľ��ϴ� C++ �Լ�
Napi::Value ParseDWG(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
      Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
      return Napi::String::New(env, "Error1");
  }
  Napi::Array jsonObj = Napi::Array::New(env);
  // Napi::Object jsonObj = Napi::Object::New(env);
  // Napi::Object jsonObj = createObject(env);
  printf("start %s \n", checkLayerName);
  std::string filePath = info[0].As<Napi::String>().Utf8Value();
  if (!info[1].IsArray()) {
      std::string layerName = info[1].As<Napi::String>().Utf8Value();
      checkLayerName = const_cast<char*>(layerName.c_str());
      if(strcmp(checkLayerName, "") != 0) {
        printf("checkLayerName: %s \n", checkLayerName);
        charArray.push_back(checkLayerName);
      }
  } else {
    Napi::Array inputArray = info[1].As<Napi::Array>();
    uint32_t length = inputArray.Length();

    // �迭 ��Ҹ� ��ȸ�ϸ� ��ȯ
    for (uint32_t i = 0; i < length; i++) {
        Napi::Value element = inputArray[i];

        if (element.IsString()) {
            Napi::String napiStr = element.As<Napi::String>();
            std::string cppStr = napiStr.Utf8Value();  // C++ ���ڿ��� ��ȯ

            // char*�� ��ȯ
            char* charPtr = new char[cppStr.length() + 1]; // +1 for null terminator
            std::strcpy(charPtr, cppStr.c_str());
            charArray.push_back(charPtr);
        } else {
            // ���ڿ��� �ƴ� ��� null ������ �߰�
            charArray.push_back(nullptr);
        }
    }
  }
  
  printf("filePath: %s \n", filePath.c_str());
  Dwg_Data dwg;
  dwg.opts = 0;
  printf("setData \n");
  dwg_read_file(filePath.c_str(), &dwg);
  CheckObjectRef(&dwg, jsonObj, env);
  // CheckLayer(&dwg);
  ExtractDWGData(dwg, jsonObj, env);
  dwg_free(&dwg);
  clearCharArray();
  // free(checkLayerName);
  return jsonObj;
}

Napi::Value ParseLayer(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
      Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
      return Napi::String::New(env, "Error1");
  }
  Napi::Object jsonObj = Napi::Object::New(env);
  // Napi::Object jsonObj = createObject(env);
  printf("start \n");
  std::string filePath = info[0].As<Napi::String>().Utf8Value();
  printf("filePath: %s \n", filePath.c_str());
  Dwg_Data dwg;
  dwg.opts = 0;
  printf("setData \n");
  dwg_read_file(filePath.c_str(), &dwg);
  // CheckObjectRef(&dwg, jsonObj, env);
  CheckLayer(&dwg, jsonObj, env);
  // ExtractDWGData(dwg, jsonObj, env);
  dwg_free(&dwg);
  return jsonObj;
}

// Node.js�� ����� ����Ƽ�� �ֵ�� ����
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "parseDWG"), Napi::Function::New(env, ParseDWG));
  exports.Set(Napi::String::New(env, "getLayer"), Napi::Function::New(env, ParseLayer));
  return exports;
}

NODE_API_MODULE(addon, Init)