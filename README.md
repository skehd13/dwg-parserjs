# 1. libreDWG설치
libreDWG를 직접 PC에 설치해야 사용 가능합니다.
1. [libreDWG다운로드](https://github.com/LibreDWG/libredwg)
2. libreDWG설치
## libreDWG 설치방법

### OSX

```
brew install autoconf automake gcc make texinfo
git clone https://github.com/LibreDWG/libredwg.git
cd libredwg
./autogen.sh
./configure
make
sudo make install
```
### windows
windows에서는 cmd 환경에서 실행
- cmake필요  
```
git clone https://github.com/LibreDWG/libredwg.git
cd libredwg
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
cmake --install .
```
### windows에서는 시스템 변수 설정필요
설치된 라이브러리를 윈도우 시스템 PATH 설정
설정 - 시스템 - 고급 시스템 설정 - 고급 탭 - 환경변수 - 시스템 변수 - PATH항목에 해당 라이브러리 경로 추가
라이브러리 경로 
- %PROGRAMFIELS(x86)%\libredwg\bin
- %PROGRAMFIELS(x86)%\libredwg\libredwg-

두개를 추가한다.
추가 이후 cmd 창을 새로 열어 

	dwgread
명령어가 동작하는 지 확인

# 설치방법 
	npm install dwg-parserjs

# 함수
|함수|설명|
|---|---|
|getLayer|dwg파일에 존재하는 layer이름을 불러오는 함수|
|parseDWG|dwg파일에서 object를 추출하는 함수|

## getLayer(filePath: string)
### options
filePath: dwg 파일 경로

### response
layerNames: string[] 
레이어 이름 배열


## parseDWG(filePath: string, layerName: string[])
### options
filePath: dwg 파일 경로
layerName: object를 추출할 레이어이름

### response
Layer[]

#### Layer

|key|Desc|
|----|---|
|layer_name|레이어 이름, blockHeader|
|component|Component집합, blockHeader집합|

#### Component
|Key|Desc|
|-----|----|
|arc|원형곡선|
|circle|원|
|ellipse|타원|
|line|선|
|lwPolyline|물리선|
|solid|솔리드|
|text|텍스트|
|mtext|여러줄텍스트|
|insert|참조객체|
|hatch|해치|
|viewPosition|화면중심|

#### blockHeaderComponent
|key|type|Desc|
|---|----|----|
|${blockHeaderName}|{flag:number, component: Layer[]}|참조되는 이름|


## 파싱타입
### arc
|key|type|desc|
|---|---|--|
|center|number[]|중심좌표|
|radius|number|반지름|
|start_angle|number|시작각도|
|end_angle|number|종료각도|

### circle
|key|type|desc|
|---|---|--|
|center|number[]|중심좌표|
|radius|number|반지름|

### ellipse

|key|type|desc|
|---|---|--|
|center|number[]|중심좌표|
|start_angle|number|시작각도|
|end_angle|number|종료각도|
|sm_axis|number[]|각축별 반지름|


### line
|key|type|desc|
|---|---|--|
|start|number[]|시작좌표|
|end|number[]|끝좌표|

### lwPolyline
|key|type|desc|
|---|---|--|
|points|number[][]|각 연결점|

### solid
|key|type|desc|
|---|---|--|
|corner1|number[]|점1|
|corner2|number[]|점2|
|corner3|number[]|점3|
|corner4|number[]|점4|

### text
|key|type|desc|
|---|---|--|
|center|number[]|중심좌표|
|rotation|number|회전값|
|text|stirng|텍스트|
|size|number|텍스트높이(사이즈)|

### mtext
|key|type|desc|
|---|---|--|
|center|number[]|중심좌표|
|rotation|number|회전값|
|text|stirng|텍스트|
|size|number|텍스트높이(사이즈)|
|width|number|가로크기|

### insert
|key|type|desc|
|----|----|----|
|base|number[]|기준좌표|
|scale|number[]|확대값|
|rotation|number|회전값|
|block_name|string|blockHeader에서 찾을 이름|


### hatch
|key|type|desc|
|----|----|----|
|name|string|해치이름|
|is_solid_fill|boolean|해당해치가 Solid인지|
|path|IHatchPath[]|해치 경로배열|

### hatchPath
|key|type|desc|
|----|----|----|
|flag|number|flag|
|seg|IHatchPathSeg[]|해치경로 seg|
|line|number[]|선경로 x,y,z순의 number 배열|

### viewPosition
화면 중앙 좌표값
