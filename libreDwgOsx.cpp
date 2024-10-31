#include <napi.h>
#include <iostream>
#include <dwg.h>
#include <dwg_api.h>
#include <math.h>
#include <string>

uint32_t zero = 0;
char* checkLayerName;
std::vector<char*> charArray;

std::string parseChar(char* text);
std::string parseChar(unsigned char* text);
std::string parseChar(unsigned char text);
Napi::Array checkObject(Napi::Object jsonObj, std::string key);
// 해치 파싱
void parseEntityHatch(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 솔리드 파싱
void parseEntitySolid(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 타원파싱
void parseEntityARC(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// M텍스트 파싱
void parseEntityMText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 텍스트 파싱
void parseEntityText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 해치 파싱
void parseEntitySpline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// X선 파싱
void parseEntityXline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 광선 파싱
void parseEntityRay(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 점 파싱
void parseEntityPoint(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 블록 파싱
void parseBlock(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 블록헤더 파싱
void parseBlockHeader(Dwg_Object_Object* object_object, Napi::Object jsonObj, Napi::Env env, bool isCheckLayer);
// Insert 파싱
void parseEntityInsert(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 선 파싱
void parseEntityLine(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 원 파싱
void parseEntityCircle(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 물리선 파싱
void parseEntityLwPolyline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// Polyline2D 파싱
void parseEntityPolyline2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// Vertex2D 파싱
void parseEntityVertex2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
// 타원(?) 파싱
void parseEntityEllipse(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);

void parseObjectBlockHeader(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
std::string ExtractDWGData(Dwg_Data* dwg, Napi::Array jsonArr, Napi::Env env);
void parseDWGObject(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer);
std::string CheckObjectRef(Dwg_Data *restrict dwg, Napi::Array jsonArr, Napi::Env env);
char* EntityLayerName(Dwg_Object* object);
char* EntityTextGetText(Dwg_Entity_TEXT* ent_text);
char * bit_convert_TU (const BITCODE_TU restrict wstr);
std::string CheckLayer(Dwg_Data *restrict dwg, Napi::Object jsonObj, Napi::Env env);
Napi::Value ParseDWG(const Napi::CallbackInfo& info);

bool contains(const std::vector<char*>& charArray, const char* target) {
  if (charArray.empty()) {
        return true;  // 비어 있으면 false 반환
    }
    // std::find_if를 사용하여 target과 같은 문자열을 찾습니다.
    auto it = std::find_if(charArray.begin(), charArray.end(), 
        [target](const char* str) {
            return strcmp(str, target) == 0;  // 문자열 비교
        });

    return it != charArray.end();  // 찾으면 true, 아니면 false
}

void clearCharArray() {
    for (char* str : charArray) {
        delete[] str; // 각 포인터로 할당된 메모리 해제
    }
    charArray.clear(); // 벡터 비우기 (선택 사항)
}

bool contains(char* arr[], int length, const char* target) {
    for (int i = 0; i < length; ++i) {
        if (strcmp(arr[i], target) == 0) {
            return true;  // 찾으면 true 반환
        }
    }
    return false;  // 찾지 못하면 false 반환
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
  if(jsonObj.Has(Napi::String::New(env, key)) == false) {
    jsonObj.Set(key, Napi::Array::New(env));
  } 
  array = jsonObj.Get(key).As<Napi::Array>();
  return array;
}

void parseEntityHatch(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_HATCH* hatch = object->tio.entity->tio.HATCH;
  char *layer_name = EntityLayerName(object);
  
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array hatchArray = checkObject(jsonObj, "hatch", env);
  layer_name = nullptr;
  Napi::Object hatchObj = Napi::Object::New(env);
  BITCODE_BL is_gradient_fill = hatch->is_gradient_fill;
  // hatchObj.Set("is_gradient_fill", Napi::Number::New(env, is_gradient_fill));
  // BITCODE_BL reserved= hatch->reserved;
  // hatchObj.Set("reserved", Napi::Number::New(env, reserved));
  // BITCODE_BD gradient_angle= hatch->gradient_angle;
  // hatchObj.Set("gradient_angle", Napi::Number::New(env, gradient_angle));
  // BITCODE_BD gradient_shift= hatch->gradient_shift;
  // hatchObj.Set("gradient_shift", Napi::Number::New(env, gradient_shift));
  // BITCODE_BL single_color_gradient= hatch->single_color_gradient;
  // hatchObj.Set("single_color_gradient", Napi::Number::New(env, single_color_gradient));
  // BITCODE_BD gradient_tint= hatch->gradient_tint;
  // hatchObj.Set("gradient_tint", Napi::Number::New(env, gradient_tint));
  // BITCODE_BL num_colors= hatch->num_colors;
  // hatchObj.Set("num_colors", Napi::Number::New(env, num_colors));
  // Dwg_HATCH_Color* colors= hatch->colors;
  // BITCODE_T gradient_name= hatch->gradient_name; 
  // std::string str_gradient_name = parseChar(gradient_name);
  // hatchObj.Set("double_flag", Napi::String::New(env, str_gradient_name));
  /* 1: SPHERICAL, 2: HEMISPHERICAL, 3: CURVED, 4: LINEAR, 5: CYLINDER */

  BITCODE_TV name= hatch->name;
  std::string str_name = parseChar(name);
  hatchObj.Set("name", Napi::String::New(env, str_name));
  BITCODE_B is_solid_fill= hatch->is_solid_fill;
  hatchObj.Set("is_solid_fill", is_solid_fill);
  // BITCODE_B is_associative= hatch->is_associative;
  // hatchObj.Set("is_associative", is_associative);
  BITCODE_BL num_paths= hatch->num_paths;
  // hatchObj.Set("num_paths", Napi::Number::New(env, num_paths));
  Napi::Array pathArray = Napi::Array::New(env);
  Dwg_HATCH_Path *paths= hatch->paths; 
  for(BITCODE_BL i = 0; i <num_paths; i++){
    Napi::Object pathObject = Napi::Object::New(env);
    pathObject.Set("flag", Napi::Number::New(env, paths->flag));
    Napi::Array segArray = Napi::Array::New(env);
    // Napi::Array lineArray = Napi::Array::New(env);
    // Napi::Array arcArray = Napi::Array::New(env);
    Dwg_HATCH_PathSeg *segs = paths->segs;
    bool isSegEnd = false;
    bool isPolyPathEnd = false;
    for(BITCODE_BL j = 0; j < paths->num_segs_or_paths; j++){
      Napi::Object segObject = Napi::Object::New(env);
      Dwg_HATCH_PolylinePath *polyline_paths = &paths->polyline_paths[j];
      // uint32_t lineLength = lineArray.Length();
      if(polyline_paths != nullptr && isPolyPathEnd != true) {
        Napi::Array polyline_paths_point = Napi::Array::New(env);
        polyline_paths_point.Set(zero, Napi::Number::New(env, polyline_paths->point.x));
        polyline_paths_point.Set(1, Napi::Number::New(env, polyline_paths->point.y));
        polyline_paths_point.Set(2, Napi::Number::New(env, 0));

        // lineArray.Set(lineLength + zero, Napi::Number::New(env, polyline_paths->point.x));
        // lineArray.Set(lineLength + 1, Napi::Number::New(env, polyline_paths->point.y));
        // lineArray.Set(lineLength + 2, Napi::Number::New(env, 0));
        segObject.Set("polyline_paths", polyline_paths_point);
      } else {
        isPolyPathEnd = true;
      }
      if(segs != nullptr && isSegEnd != true){
        BITCODE_RC curve_type = segs->curve_type;
        segObject.Set("curve_type", Napi::Number::New(env, segs->curve_type));
        if(curve_type == 1) {
          Napi::Array first_endpoint = Napi::Array::New(env, 2);
          first_endpoint.Set(zero, Napi::Number::New(env, segs->first_endpoint.x));
          first_endpoint.Set(1, Napi::Number::New(env, segs->first_endpoint.y));
          segObject.Set("first_endpoint", first_endpoint);
          Napi::Array second_endpoint = Napi::Array::New(env, 2);
          second_endpoint.Set(zero, Napi::Number::New(env, segs->second_endpoint.x));
          second_endpoint.Set(1, Napi::Number::New(env, segs->second_endpoint.y));
          segObject.Set("second_endpoint", second_endpoint);
        } else if(curve_type == 2) {
          Napi::Array center = Napi::Array::New(env, 2);
          center.Set(zero, Napi::Number::New(env, segs->center.x));
          center.Set(1, Napi::Number::New(env, segs->center.y));
          segObject.Set("center", center);
          segObject.Set("radius", Napi::Number::New(env, segs->radius));
          segObject.Set("start_angle", Napi::Number::New(env, segs->start_angle));
          segObject.Set("end_angle", Napi::Number::New(env, segs->end_angle));
        // } else if(curve_type == 3) {
        //   Napi::Array endpoint = Napi::Array::New(env, 2);
        //   endpoint.Set(zero, Napi::Number::New(env, segs->endpoint.x));
        //   endpoint.Set(1, Napi::Number::New(env, segs->endpoint.y));
        //   segObject.Set("endpoint", endpoint);
        // } else if(curve_type == 4) {
        //   segObject.Set("end_angle", Napi::Number::New(env, segs->degree));
        //   segObject.Set("end_angle", Napi::Number::New(env, segs->is_rational));
        //   segObject.Set("end_angle", Napi::Number::New(env, segs->is_periodic));
        //   Napi::Array knots = Napi::Array::New(env, segs->num_knots);
        //   BITCODE_BD* knot = segs->knots;
        //   for(BITCODE_BL k = 0; k < segs->num_knots; k++) {
        //     knots.Set(k, Napi::Number::New(env, *knot));
        //     knot++;
        //   }
        //   segObject.Set("knots", knots);
        //   BITCODE_BL num_fitpts = segs->num_fitpts;
        //   Napi::Array fitptsArr = Napi::Array::New(env, segs->num_fitpts);
        //   BITCODE_2RD* fitpts = segs->fitpts;
        //   for(BITCODE_BL k = 0; k < num_fitpts; k++) {
        //     BITCODE_2RD fitpt = fitpts[k];
        //     Napi::Array fitptRd = Napi::Array::New(env, 3);
        //     fitptRd.Set(zero, fitpt.x);
        //     fitptRd.Set(1, fitpt.y);
        //     fitptRd.Set(2, 0);
        //     fitptsArr.Set(k, fitptRd);
        //   }
        //   segObject.Set("fitpts", fitptsArr);
        //   Napi::Array start_tangent = Napi::Array::New(env, 2);
        //   start_tangent.Set(zero, Napi::Number::New(env, segs->start_tangent.x));
        //   start_tangent.Set(1, Napi::Number::New(env, segs->start_tangent.y));
        //   segObject.Set("start_tangent", start_tangent);
        //   Napi::Array end_tangent = Napi::Array::New(env, 2);
        //   end_tangent.Set(zero, Napi::Number::New(env, segs->end_tangent.x));
        //   end_tangent.Set(1, Napi::Number::New(env, segs->end_tangent.y));
        //   segObject.Set("end_tangent", end_tangent);
        }
      } else {
        isSegEnd = true;
      }
      segArray.Set(j, segObject);
      segs++;
    }
    segs = nullptr;
    paths++;
    pathObject.Set("seg", segArray);
    // pathObject.Set("line", lineArray);
    // pathObject.Set("arc", arcArray);
    pathArray.Set(i, pathObject);
  }
  paths = nullptr;
  hatchObj.Set("path", pathArray);
  // also named loop
  // BITCODE_BS style= hatch->style;
  // hatchObj.Set("style", Napi::Number::New(env, style));
  // BITCODE_BS pattern_type= hatch->pattern_type;
  // hatchObj.Set("pattern_type", Napi::Number::New(env, pattern_type));
  // BITCODE_BD angle= hatch->angle;
  // hatchObj.Set("angle", Napi::Number::New(env, angle));
  // BITCODE_BD scale_spacing= hatch->scale_spacing;
  // hatchObj.Set("scale_spacing", Napi::Number::New(env, scale_spacing));
  // BITCODE_B double_flag= hatch->double_flag;
  // hatchObj.Set("double_flag", Napi::Number::New(env, double_flag));
  // BITCODE_BS num_deflines= hatch->num_deflines;
  // hatchObj.Set("num_deflines", Napi::Number::New(env, num_deflines));
  // Napi::Array deflinesArray = Napi::Array::New(env);
  // Dwg_HATCH_DefLine * deflines= hatch->deflines;
  // for(BITCODE_BS i =0; i < hatch->num_deflines; i++){
  //   Napi::Object deflinesObject = Napi::Object::New(env);
  //   deflinesObject.Set("angle", Napi::Number::New(env,deflines->angle));
  //   Napi::Array pt0 = Napi::Array::New(env, 2);
  //   pt0.Set(zero, deflines->pt0.x);
  //   pt0.Set(1, deflines->pt0.y);
  //   deflinesObject.Set("pt0", pt0);
  //   Napi::Array offset = Napi::Array::New(env, 2);
  //   offset.Set(zero, deflines->offset.x);
  //   offset.Set(1, deflines->offset.y);
  //   deflinesObject.Set("offset", offset);
  //   BITCODE_BD* dashes = deflines->dashes;
  //   Napi::Array dashesArray = Napi::Array::New(env, 2);
  //   for(BITCODE_BS j = 0; j < deflines->num_dashes; j++) {
  //     dashesArray.Set(j, Napi::Number::New(env, *dashes));
  //     dashes++;
  //   }
  //   deflinesObject.Set("dashes", dashesArray);
  //   deflinesArray.Set(i, deflinesObject);
  //   deflines++;
  // }
  // hatchObj.Set("deflines", deflinesArray);
  
  uint32_t length = hatchArray.Length();
  hatchArray.Set(length, hatchObj);
  // jsonObj.Set("hatch", hatchArray);
  hatch = nullptr;
  return;
}

void parseEntitySolid(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_SOLID* solid = object->tio.entity->tio.SOLID;
  char *layer_name = EntityLayerName(object);
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array solidArray = checkObject(jsonObj, "solid", env);
  layer_name = nullptr;
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
  // jsonObj.Set("solid", solidArray);
  solid = nullptr;
  return;
}

void parseEntityARC(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_ARC* arc = object->tio.entity->tio.ARC;
  
  char *layer_name = EntityLayerName(object);
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array arcArray = checkObject(jsonObj, "arc", env);
  layer_name = nullptr;
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
  // jsonObj.Set("arc", arcArray);
  arc = nullptr;
  return;
}

// M텍스트
void parseEntityMText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_MTEXT* mtext = object->tio.entity->tio.MTEXT;
  char *layer_name = EntityLayerName(object);
   if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array mtextArray = checkObject(jsonObj, "mtext", env);
  layer_name = nullptr;
  Napi::Object mtextObj = Napi::Object::New(env);
  Napi::Array centerArray = Napi::Array::New(env, 3);
  centerArray.Set(zero, Napi::Number::New(env, mtext->ins_pt.x));
  centerArray.Set(1, Napi::Number::New(env, mtext->ins_pt.y));
  centerArray.Set(2, Napi::Number::New(env, mtext->ins_pt.z));
  mtextObj.Set("center", centerArray);\
  double rotation = atan2(mtext->x_axis_dir.y,mtext->x_axis_dir.x);
  mtextObj.Set("rotation", rotation);
  
  BITCODE_BD text_height = mtext->text_height;
  BITCODE_BD extents_width = mtext->extents_width;
  
  char* text = bit_convert_TU((BITCODE_TU)mtext->text);
  mtextObj.Set("text", Napi::String::New(env, text));
  text = nullptr;
  mtextObj.Set("size", text_height);
  mtextObj.Set("width", extents_width);
  uint32_t length = mtextArray.Length();
  mtextArray.Set(length, mtextObj);
  // jsonObj.Set("mtext", mtextArray);
  mtext = nullptr;
  return;
}

// 텍스트
void parseEntityText(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_TEXT* text = object->tio.entity->tio.TEXT;
  char *layer_name = EntityLayerName(object);
  
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array textArray = checkObject(jsonObj, "text", env);
  layer_name = nullptr;
  Napi::Object textObj = Napi::Object::New(env);
  Napi::Array centerArray = Napi::Array::New(env, 3);
  centerArray.Set(zero, Napi::Number::New(env, text->ins_pt.x));
  centerArray.Set(1, Napi::Number::New(env, text->ins_pt.y));
  centerArray.Set(2, Napi::Number::New(env, 0));
  BITCODE_RD rotation = text->rotation;
  BITCODE_RD width = text->width_factor;
  BITCODE_RD height = text->height;
  textObj.Set("rotation", rotation);
  textObj.Set("center", centerArray);
  textObj.Set("width", width);
  textObj.Set("size", height);
  char* strText = EntityTextGetText(text);
  textObj.Set("text", Napi::String::New(env, strText));
  strText = nullptr;
  uint32_t length = textArray.Length();
  textArray.Set(length, textObj);
  // jsonObj.Set("text", textArray);
  text = nullptr;
  return;
}

// 곡선
void parseEntitySpline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_SPLINE* spline = object->tio.entity->tio.SPLINE;
  char *layer_name = EntityLayerName(object);
  
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array splineArray = checkObject(jsonObj, "spline", env);
  layer_name = nullptr;
  Napi::Object splineObj = Napi::Object::New(env);
  Napi::Array points = Napi::Array::New(env);
  BITCODE_3DPOINT* fit_pts = spline->fit_pts;
  for(BITCODE_BS i = 0; i < spline->num_fit_pts; i++){
    Napi::Array point = Napi::Array::New(env);
    point.Set(zero, Napi::Number::New(env, fit_pts->x));
    point.Set(1, Napi::Number::New(env, fit_pts->y));
    point.Set(2, Napi::Number::New(env, fit_pts->z));
    points.Set(i, point);
    fit_pts++; // 포인터를 다음 요소로 이동
  }
  fit_pts = nullptr;

  splineObj.Set("points", points);
  uint32_t length = splineArray.Length();
  splineArray.Set(length, splineObj);
  // jsonObj.Set("spline", splineArray);
  spline = nullptr;
  return;
}

// X선
void parseEntityXline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_XLINE* xline = object->tio.entity->tio.XLINE;
  char *layer_name = EntityLayerName(object);
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array xlineArray = checkObject(jsonObj, "xline", env);
  layer_name = nullptr;
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
  // jsonObj.Set("xline", xlineArray);
  xline = nullptr;
  return;
}

// 광선
void parseEntityRay(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_RAY* ray = object->tio.entity->tio.RAY;
  char *layer_name = EntityLayerName(object);
    
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array rayArray = checkObject(jsonObj, "ray", env);
  layer_name = nullptr;
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
  // jsonObj.Set("ray", rayArray);
  ray = nullptr;
  return;
}

//점
void parseEntityPoint(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_POINT* point = object->tio.entity->tio.POINT;
  char *layer_name = EntityLayerName(object);
    
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  Napi::Array pointArray = checkObject(jsonObj, "point", env);
  layer_name = nullptr;
  Napi::Object pointObj = Napi::Object::New(env);
  Napi::Array pointArr = Napi::Array::New(env);
  pointArr.Set(zero, Napi::Number::New(env, point->x));
  pointArr.Set(1, Napi::Number::New(env, point->y));
  pointArr.Set(2, Napi::Number::New(env, point->z));
  
  pointObj.Set("point", pointArr);
  uint32_t length = pointArray.Length();
  pointArray.Set(length, pointObj);
  // jsonObj.Set("point", pointArray);
  point = nullptr;
  return;
}

void parseBlock(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_BLOCK* block = object->tio.entity->tio.BLOCK;
  char *layer_name = EntityLayerName(object);
    
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;

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
  // jsonObj.Set("block", blockArray);
  block = nullptr;
  return;
}

void parseObjectBlockHeader(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Object_BLOCK_HEADER* blockHeader = object->tio.object->tio.BLOCK_HEADER;
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)"blockHeader", env);
  std::string name = parseChar(blockHeader->name);
  char checkName = '*';
  if(name[0] == checkName){
    return;
  }
  Napi::Object blockHeaderObject = Napi::Object::New(env);
  blockHeaderObject.Set("flag", static_cast<int>(blockHeader->flag));
  BITCODE_BL num_owned = blockHeader->num_owned;
  Napi::Array blockHeaderComponent = Napi::Array::New(env);
  
  if(static_cast<int>(blockHeader->flag) < 68){
    BITCODE_H* entities = blockHeader->entities;
    for(BITCODE_BL i = 0; i < num_owned; i++) {
      Dwg_Object* object = entities[i]->obj;
      if(object != nullptr){
        parseDWGObject(object, blockHeaderComponent, env, false);
      }else{
      }
      object = nullptr;
    }
    entities = nullptr;
  }

  blockHeaderObject.Set("component", blockHeaderComponent);
  jsonObj.Set(name, blockHeaderObject);
  blockHeader = nullptr;
  return;
}

void parseEntityInsert(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_INSERT* insert = object->tio.entity->tio.INSERT;
  char *layer_name = EntityLayerName(object);
    
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;
  Napi::Array insertArray = checkObject(jsonObj, "insert", env);
  Napi::Object insertObject = Napi::Object::New(env);
  Napi::Array baseArray = Napi::Array::New(env, 3);
  BITCODE_3DPOINT   ins_pt = insert->ins_pt;
  baseArray.Set(zero, Napi::Number::New(env, ins_pt.x));
  baseArray.Set(1, Napi::Number::New(env, ins_pt.y));
  baseArray.Set(2, Napi::Number::New(env, ins_pt.z));
  insertObject.Set("base", baseArray);
  Napi::Array scaleArray = Napi::Array::New(env, 3);
  BITCODE_3DPOINT   scale = insert->scale;
  scaleArray.Set(zero, Napi::Number::New(env, scale.x));
  scaleArray.Set(1, Napi::Number::New(env, scale.y));
  scaleArray.Set(2, Napi::Number::New(env, scale.z));
  insertObject.Set("scale", scaleArray);
  BITCODE_BD        rotation = insert->rotation;
  insertObject.Set("rotation", Napi::Number::New(env, rotation));
  Dwg_Object_BLOCK_HEADER* blockHeader = insert->block_header->obj->tio.object->tio.BLOCK_HEADER;
  std::string block_header_name = parseChar(blockHeader->name);
  insertObject.Set("block_name", block_header_name);
  uint32_t length = insertArray.Length();
  insertArray.Set(length, insertObject);
  // jsonObj.Set("insert", insertArray);
  insert = nullptr;
  return;
}

// 선
void parseEntityLine(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_LINE* line = object->tio.entity->tio.LINE;
  char *layer_name = EntityLayerName(object);
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;
  Dwg_Bitcode_3BD start = line->start;
  Dwg_Bitcode_3BD end = line->end;
  Napi::Array lineArray = checkObject(jsonObj, "line", env);
  
  Napi::Object lineObj = Napi::Object::New(env);
  Napi::Array startArray = Napi::Array::New(env, 3);
  Napi::Array endArray = Napi::Array::New(env, 3);
  startArray.Set(zero, Napi::Number::New(env, start.x));
  startArray.Set(1, Napi::Number::New(env, start.y));
  startArray.Set(2, Napi::Number::New(env, start.z));

  // 'end' 배열에 값 설정
  endArray.Set(zero, Napi::Number::New(env, end.x));
  endArray.Set(1, Napi::Number::New(env, end.y));
  endArray.Set(2, Napi::Number::New(env, end.z));

  // 'lineObj' 객체에 'start'와 'end' 배열을 설정
  lineObj.Set("start", startArray);
  lineObj.Set("end", endArray);
  uint32_t length = lineArray.Length();

  Napi::Array newLineArray = Napi::Array::New(env, length + 1);
  for (uint32_t i = 0; i < length; ++i) {
    newLineArray.Set(i, lineArray.Get(i));
  }
  newLineArray.Set(length, lineObj);
  // jsonObj.Set("line", newLineArray);
  line = nullptr;
  return;
}

// 원
void parseEntityCircle(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_CIRCLE* circle = object->tio.entity->tio.CIRCLE;
  char *layer_name = EntityLayerName(object);
  
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;
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
  // jsonObj.Set("circle", circleArray);
  circle = nullptr;
  return;
}

// 물리선
void parseEntityLwPolyline(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_LWPOLYLINE* lwPolyline = object->tio.entity->tio.LWPOLYLINE;
  char *layer_name = EntityLayerName(object);
  
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;
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
    points++; // 포인터를 다음 요소로 이동
  }
  points = nullptr;
  pointObj.Set("point", pointArray);
  uint32_t length = lwPolylineArray.Length();
  lwPolylineArray.Set(length, pointObj);
  // jsonObj.Set("lwPolyline", lwPolylineArray);
  lwPolyline = nullptr;
  return;
}

void parseEntityPolyline2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_POLYLINE_2D* polyline2D = object->tio.entity->tio.POLYLINE_2D;
  char *layer_name = EntityLayerName(object);
    
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;
  Napi::Array polyline2DArray = checkObject(jsonObj, "polyline2D", env);
  Napi::Object polyline2DObject = Napi::Object::New(env);
  Napi::Array polyline2DComponent = Napi::Array::New(env);
  BITCODE_BL num_owned = polyline2D->num_owned;
  BITCODE_H first_vertex = polyline2D->first_vertex;
  if(first_vertex != nullptr){
    parseDWGObject(first_vertex->obj, polyline2DComponent, env, false);
  }
  BITCODE_H last_vertex = polyline2D->last_vertex;
  if(last_vertex != nullptr){
    parseDWGObject(last_vertex->obj, polyline2DComponent, env, false);
  }
  BITCODE_H *vertex = polyline2D->vertex;
  for(BITCODE_BL i = 0; i < num_owned; i++) {
    Dwg_Object* object = vertex[i]->obj;
    parseDWGObject(object, polyline2DComponent, env, false);
  }
  BITCODE_H seqend = polyline2D->seqend;
  if(seqend != nullptr){
    parseDWGObject(seqend->obj, polyline2DComponent, env, false);
  }
  BITCODE_TV extra_r11_text = polyline2D->extra_r11_text;
  if(extra_r11_text != nullptr){
    std::string str_extra_r11_text = parseChar(extra_r11_text);
  }
  polyline2DObject.Set("component", polyline2DComponent);
  uint32_t length = polyline2DArray.Length();
  polyline2DArray.Set(length, polyline2DObject);
  // jsonObj.Set("polyline2D", polyline2DArray);
  polyline2D = nullptr;
  return;
}

void parseEntityVertex2D(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_VERTEX_2D* vertex2D = object->tio.entity->tio.VERTEX_2D;
  char *layer_name = EntityLayerName(object);
    
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }

  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;
  Napi::Array vertex2DArray = checkObject(jsonObj, "vertex2D", env);
  Napi::Object vertex2DObject = Napi::Object::New(env);
  BITCODE_3BD point = vertex2D->point;
  Napi::Array pointArray = Napi::Array::New(env);
  pointArray.Set(zero, Napi::Number::New(env, point.x));
  pointArray.Set(1, Napi::Number::New(env, point.y));
  pointArray.Set(2, Napi::Number::New(env, point.z));
  vertex2DObject.Set("point", pointArray);
  uint32_t length = vertex2DArray.Length();
  vertex2DArray.Set(length, vertex2DObject);
  // jsonObj.Set("vertex2D", vertex2DArray);
  vertex2D = nullptr;
  return;
}

// 타원(?) 파싱
void parseEntityEllipse(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
  Dwg_Entity_ELLIPSE* ellipse = object->tio.entity->tio.ELLIPSE;
  char *layer_name = EntityLayerName(object);
    
  if(!contains(charArray, layer_name) && isCheckLayer) {
    return;
  }
  Napi::Object jsonObj = checkArray(jsonArr, (std::string)layer_name, env);
  layer_name = nullptr;
  Napi::Array ellipseArray = checkObject(jsonObj, "ellipse", env);
  Napi::Object ellipseObject = Napi::Object::New(env);

  BITCODE_3BD center = ellipse->center;
  Napi::Array centerArray = Napi::Array::New(env);
  centerArray.Set(zero, Napi::Number::New(env, center.x));
  centerArray.Set(1, Napi::Number::New(env, center.y));
  centerArray.Set(2, Napi::Number::New(env, center.z));
  ellipseObject.Set("center", centerArray);
  BITCODE_3BD sm_axis = ellipse->sm_axis;
  Napi::Array axisArray = Napi::Array::New(env);
  axisArray.Set(zero, Napi::Number::New(env, sm_axis.x));
  axisArray.Set(1, Napi::Number::New(env, sm_axis.y));
  axisArray.Set(2, Napi::Number::New(env, sm_axis.z));
  ellipseObject.Set("sm_axis", axisArray);
  BITCODE_BE extrusion = ellipse->extrusion;
  Napi::Array extrusionArray = Napi::Array::New(env);
  extrusionArray.Set(zero, Napi::Number::New(env, extrusion.x));
  extrusionArray.Set(1, Napi::Number::New(env, extrusion.y));
  extrusionArray.Set(2, Napi::Number::New(env, extrusion.z));
  ellipseObject.Set("extrusion", extrusionArray);
  BITCODE_BD axis_ratio = ellipse->axis_ratio; 
  ellipseObject.Set("axis_ratio", Napi::Number::New(env, axis_ratio));
  BITCODE_BD start_angle = ellipse->start_angle;
  ellipseObject.Set("start_angle", Napi::Number::New(env, start_angle));
  BITCODE_BD end_angle = ellipse->end_angle;
  ellipseObject.Set("end_angle", Napi::Number::New(env, end_angle));

  uint32_t length = ellipseArray.Length();
  ellipseArray.Set(length, ellipseObject);
  // jsonObj.Set("ellipse", ellipseArray);
  ellipse = nullptr;
  return;
}

std::string ExtractDWGData(Dwg_Data* dwg, Napi::Array jsonArr, Napi::Env env) {
  double modelxmin = dwg_model_x_min(dwg);
  double modelxmax = dwg_model_x_max(dwg);
  double modelymin = dwg_model_y_min(dwg);
  double modelymax = dwg_model_y_max(dwg);
  double modelzmin = dwg_model_z_min(dwg);
  double modelzmax = dwg_model_z_max(dwg);
  Napi::Object jsonObj = checkArray(jsonArr, "viewPosition", env);
  Napi::Array viewPosition = checkObject(jsonObj,"viewPosition", env);
  
  viewPosition.Set(zero, Napi::Number::New(env, (modelxmax+modelxmin)/2));
  viewPosition.Set(1, Napi::Number::New(env, (modelymax+modelymin)/2));
  viewPosition.Set(2, Napi::Number::New(env, (modelzmax+modelzmin)/2));
  // jsonObj.Set("viewPosition", viewPosition);
  return std::string(dwg_version_type(dwg->header.from_version));
}

void parseDWGObject(Dwg_Object* object, Napi::Array jsonArr, Napi::Env env, bool isCheckLayer) {
    if(object){
      int type = (int)object->fixedtype;
      switch(type){
        case DWG_TYPE_ARC:
          parseEntityARC(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_LINE:
          parseEntityLine(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_CIRCLE:
          parseEntityCircle(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_POLYLINE_2D:
          parseEntityPolyline2D(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_POLYLINE_3D:
          // parseEntityLine(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_BLOCK:
          parseBlock(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_INSERT:
          parseEntityInsert(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_BLOCK_HEADER:
          parseObjectBlockHeader(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_ELLIPSE:
          parseEntityEllipse(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_POINT:
          parseEntityPoint(object, jsonArr, env, isCheckLayer);
          break;
          break;
        case DWG_TYPE_TEXT:
          parseEntityText(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_MTEXT:
          parseEntityMText(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_SPLINE:
          parseEntitySpline(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_XLINE:
          parseEntityXline(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_LWPOLYLINE:
          parseEntityLwPolyline(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_SOLID:
          parseEntitySolid(object, jsonArr, env, isCheckLayer);
          break;
        case DWG_TYPE_HATCH:
          parseEntityHatch(object, jsonArr, env, isCheckLayer);
          break;
      }
    }
    
}

std::string CheckObjectRef(Dwg_Data *restrict dwg, Napi::Array jsonArr, Napi::Env env) {
  // std::cout << "DWG dwg->num_object_refs: " << dwg->num_objects << std::endl;
  Dwg_Object *object = dwg->object;
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++){
    if(object != nullptr){
      parseDWGObject(object, jsonArr, env, true);
    }
    object++;
  }
  object = nullptr;
  return std::string("check end");
}

char* EntityLayerName(Dwg_Object* object) {
  int error;
  char *name;
  Dwg_Object_Entity *ent = object->tio.entity;
  name = dwg_ent_get_layer_name (ent, &error);
  ent = nullptr;
  if (!error){
    return name;
  }
  return name;
}

char* EntityTextGetText(Dwg_Entity_TEXT* ent_text) {
  char *text;
  text = bit_convert_TU((BITCODE_TU)ent_text->text_value);
  return text;
}

char *
bit_convert_TU (const BITCODE_TU restrict wstr)
{
  BITCODE_TU tmp = wstr;
  char *str;
  int i, len = 0;
  uint16_t c = 0;

  if (!wstr)
    return NULL;
  while ((c = *tmp++))
    {
      len++;
      if (c >= 0x80)
        {
          len++;
          if (c >= 0x800)
            len++;
        }
    }
  str = (char *)malloc (len + 1);
  if (!str)
    {
      return NULL;
    }
  i = 0;
  tmp = wstr;
  while ((c = *tmp++) && i < len)
    {
      if (c < 0x80)
        {
          str[i++] = c & 0xFF;
        }
      else if (c < 0x800)
        {
          str[i++] = (c >> 6) | 0xC0;
          str[i++] = (c & 0x3F) | 0x80;
        }
      else
        {
          str[i++] = (c >> 12) | 0xE0;
          str[i++] = ((c >> 6) & 0x3F) | 0x80;
          str[i++] = (c & 0x3F) | 0x80;
        }
    }
  if (i <= len + 1)
    str[i] = '\0';
  return str;
}

std::string CheckLayer(Dwg_Data *restrict dwg, Napi::Object jsonObj, Napi::Env env) {
  unsigned int layer_count = dwg_get_layer_count(dwg);
  Dwg_Object_LAYER ** layer_objects =  dwg_get_layers(dwg);
  Napi::Array layerNames = Napi::Array::New(env);
  int error;
  char *name;
  for (unsigned int i = 0; i < layer_count; i++)
    {
      Dwg_Object_LAYER * layer = layer_objects[i];
      if(layer){
        name = dwg_obj_layer_get_name(layer, &error);
        uint32_t length = layerNames.Length();
        layerNames.Set(length, Napi::String::New(env, name));
        free(name);
      }
      layer = nullptr;
    }
  layer_objects = nullptr;
  // jsonObj.Set("layerNames", layerNames);
  return std::string("check end");
}

// DWG 파일을 파싱하는 C++ 함수
Napi::Value ParseDWG(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
      Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
      return Napi::String::New(env, "Error1");
  }
  Napi::Array jsonObj = Napi::Array::New(env);
  std::string filePath = info[0].As<Napi::String>().Utf8Value();
  if (!info[1].IsArray()) {
      std::string layerName = info[1].As<Napi::String>().Utf8Value();
      checkLayerName = const_cast<char*>(layerName.c_str());
      if(strcmp(checkLayerName, "") != 0) {
        charArray.push_back(checkLayerName);
      }
  } else {
    Napi::Array inputArray = info[1].As<Napi::Array>();
    uint32_t length = inputArray.Length();

    // 배열 요소를 순회하며 변환
    for (uint32_t i = 0; i < length; i++) {
        Napi::Value element = inputArray[i];

        if (element.IsString()) {
            Napi::String napiStr = element.As<Napi::String>();
            std::string cppStr = napiStr.Utf8Value();  // C++ 문자열로 변환

            // char*로 변환
            char* charPtr = new char[cppStr.length() + 1]; // +1 for null terminator
            std::strcpy(charPtr, cppStr.c_str());
            charArray.push_back(charPtr);
        } else {
            // 문자열이 아닌 경우 null 포인터 추가
            charArray.push_back(nullptr);
        }
    }
  }
  Dwg_Data* dwg = new Dwg_Data();
  // dwg.opts = 0;
  printf("setData \n");
  dwg_read_file(filePath.c_str(), dwg);
  CheckObjectRef(dwg, jsonObj, env);
  ExtractDWGData(dwg, jsonObj, env);
  dwg_free(dwg);
  clearCharArray();
  dwg = nullptr;
  return jsonObj;
}

Napi::Value ParseLayer(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsString()) {
      Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
      return Napi::String::New(env, "Error1");
  }
  Napi::Object jsonObj = Napi::Object::New(env);
  std::string filePath = info[0].As<Napi::String>().Utf8Value();
  Dwg_Data* dwg = new Dwg_Data();
  // dwg.opts = 0;
  dwg_read_file(filePath.c_str(), dwg);
  CheckLayer(dwg, jsonObj, env);
  dwg_free(dwg);
  dwg = nullptr;
  return jsonObj;
}

// Node.js와 연결된 네이티브 애드온 정의
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "parseDWG"), Napi::Function::New(env, ParseDWG));
  exports.Set(Napi::String::New(env, "getLayer"), Napi::Function::New(env, ParseLayer));
  return exports;
}

NODE_API_MODULE(addon, Init)