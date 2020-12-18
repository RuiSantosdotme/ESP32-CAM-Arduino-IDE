/****************************
  This include file, index_OCV_ColorTrack.h, the Client, is an intoduction of OpenCV.js to the ESP32 Camera environment. The Client was
  developed and written by Andrew R. Sass. Permission to reproduce the index_OCV_ColorTrack.h file is granted free of charge if this
  entire copyright notice is included in all copies of the index_OCV_ColorTrack.h file. 
  
  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-opencv-js-color-detection-tracking/
*******************************/
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
   <title>ESP32-CAMERA COLOR DETECTION</title>
   <meta charset="utf-8">
   <meta name="viewport" content="width=device-width,initial-scale=1">
   <!----ANN:3--->
   <script async src=" https://docs.opencv.org/master/opencv.js" type="text/javascript"></script>
</head>
<style>
html {
    font-family: Arial, Helvetica, sans-serif;
    }
body { 
    background-color: #F7F7F2;
    margin: 0px;
}
h1 {
    font-size: 1.6rem;
    color:white;
    text-align: center;
}
.topnav {
    overflow: hidden;
    background-color: #0A1128;
}
.main-controls{
  padding-top: 5px;
}
h2 {
    color: #0A1128;
    font-size: 1rem;
}    
.section {
    margin: 2px;
    padding: 10px;
}
.column{
    float: left;
    width: 50%
}
table {
    margin: 0;
    width: 90%;
    border-collapse: collapse;
}
th{
    text-align: center;
}
.row{
    margin-right:50px;
    margin-left:50px;
}

#colorDetect{ 
    border: none;
    color: #FEFCFB;
    background-color: #0A1128;
    padding: 15px;
    text-align: center;
    display: inline-block;
    font-size: 16px;
    border-radius: 4px;
}
#restart{
    border: none;
    color: #FEFCFB;
    background-color: #7B0828;
    padding: 15px;
    text-align: center;
    display: inline-block;
    font-size: 16px;
    border-radius: 4px;  
}
button{
    border: none;
    color: #FEFCFB;
    background-color: #0A1128;
    padding: 10px;
    text-align: center;
    display: inline-block;
    border-radius: 4px;    
}

</style>
<body>
    <div class="topnav">
        <h1>ESP32-CAM Color Detection and Tracking</h1>
    </div>
    <div class="main-controls">
        <table>
            <tr>
                <td><center><input type="button" id="colorDetect" value="COLOR DETECTION"></center></td> 
                <td><center><input type="button" id="restart" value="RESET BOARD"></center></td> 
            </tr>      
        </table>
    </div>
<div class="container">
  <div class = "row"> 
    <div class = "column"> 
        <div class="section">
            <div class ="video-container">
                <h2>Video Streaming</h2>   
                <center><img id="ShowImage" src="" style="display:none"></center>
                <center><canvas id="canvas" style="display:none"></canvas></center>
            </div>
        </div>
        <div class="section">
            <table>
              <tr>
                  <td>Quality</td>
                  <td><input type="range" id="quality" min="10" max="63" value="10"></td>
              </tr>
              <tr>
                  <td>Brightness</td>
                  <td><input type="range" id="brightness" min="-2" max="2" value="0"></td>
              </tr>
              <tr>
                  <td>Contrast</td>
                  <td><input type="range" id="contrast" min="-2" max="2" value="0"></td>
              </tr>
            </table>
        </div>
 
      <!-----ANN:5---->
      <div class="section">
        <h2>RGB Color Trackbars</h2>
        <table>
            <tr>
                <td>R min:&#160;&#160;&#160;<span id="RMINdemo"></span></td>
                <td><input type="range" id="rmin" min="0" max="255" value="0" class = "slider"></td>
                <td>R max:&#160;&#160;&#160;<span id="RMAXdemo"></span></td>
                <td><input type="range" id="rmax" min="0" max="255" value="50" class = "slider"></td>
            </tr>
            <tr>
                <td>G min:&#160;&#160;&#160;<span id="GMINdemo"></span></td>
                <td><input type="range" id="gmin" min="0" max="255" value="0" class = "slider"></td>
                <td>G max:&#160;&#160;&#160;<span id="GMAXdemo"></span></td>
                <td><input type="range" id="gmax" min="0" max="255" value="50" class = "slider"></td>
            </tr>
            <tr>
                <td>B min:&#160;&#160;&#160;<span id ="BMINdemo"></span></td>
                <td><input type="range" id="bmin" min="0" max="255" value="0" class = "slider">  </td>
                <td>B max:&#160;&#160;&#160;<span id="BMAXdemo"></span></td>
                <td> <input type="range" id="bmax" min="0" max="255" value="50" class = "slider">   </td>
            </tr>
        </table>
      </div>

      <div class="section">
        <h2>Threshold Minimum-Binary Image</h2>
        <table>
            <tr>
                <td>Minimum Threshold:&#160;&#160;&#160;<span id="THRESH_MINdemo"></span></td>
                <td><input type="range" id="thresh_min" min="0" max="255" value="120" class = "slider">  </td>
            </tr>
        </table>
    </div>
     <!----ANN:9---> 
     <div class="section">
        <h2>Color Probe</h2>
        <table>
            <tr>
                <td>X probe:&#160;&#160;&#160;<span id="X_PROBEdemo"></span></td>
                <td><input type="range" id="x_probe" min="0" max="400" value="200" class = "slider"></td>
                <td>Y probe:&#160;&#160;&#160;<span id="Y_PROBEdemo"></span></td>
                <td> <input type="range" id="y_probe" min="0" max="296" value="148" class = "slider"></td>
            </tr>
        </table>
      </div>
            
    </div>   <!------endfirstcolumn---------------->   
    
    <div class = "column">      
        <div class="section">
            <h2>Image Mask</h2>
            <canvas id="imageMask"></canvas>
        </div>
        <div class="section">
            <h2>Image Canvas</h2>
            <canvas id="imageCanvas"></canvas>
        </div>
        <div class="section">
            <table>
                <tr>
                    <td><button type="button" id="invertButton" class="btn btn-primary">INVERT</button></td>
                    <td><button type="button" id="contourButton" class="btn btn-primary">SHOW CONTOUR</button></td>
                    <td><button type="button" id="trackButton" class="btn btn-primary">TRACKING</button></td>
                </tr>
                <tr>
                    <td>Invert: <span id="INVERTdemo"></span></td>
                    <td>Contour: <span id="CONTOURdemo"></span></td>
                    <td>Track: <span id="TRACKdemo"></span>
                    </td>
                </tr>
            </table>
        </div>
        <div class="section">
            <table>
                <tr>
                    <td><strong>XCM:</strong> <span id="XCMdemo"></span></td>
                    <td><strong>YCM:</strong> <span id="YCMdemo"></span></td>
                </tr>
            </table>
        </div>
        
        <div class="section">
            <canvas id="textCanvas" width="480" height="180" style= "border: 1px solid #black;"></canvas>
            <iframe id="ifr" style="display:none"></iframe>
            <div id="message"></div>  
        </div>             
        </div>  <!------end2ndcolumn------------------------>
  </div>   <!-----endrow---------------------->   
</div>   <!------endcontainer-------------->
 <!--------------- </body>----------------->
 <!----------------</html>----------------->
<div class="modal"></div>
<script>
var colorDetect = document.getElementById('colorDetect');
var ShowImage = document.getElementById('ShowImage');
var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");
var imageMask = document.getElementById("imageMask");
var imageMaskContext = imageMask.getContext("2d"); 
var imageCanvas = document.getElementById("imageCanvas");
var imageContext = imageCanvas.getContext("2d"); 
var txtcanvas = document.getElementById("textCanvas");
var ctx = txtcanvas.getContext("2d");  
var message = document.getElementById('message');
var ifr = document.getElementById('ifr');
var myTimer;
var restartCount=0;
const modelPath = 'https://ruisantosdotme.github.io/face-api.js/weights/';
let currentStream;
let displaySize = { width:400, height: 296 }
let faceDetection;

let b_tracker = false;
let x_cm = 0;
let y_cm = 0;

let b_invert = false;

let b_contour = false;

var RMAX=50;
var RMIN=0;
var GMAX=50;
var GMIN=0;
var BMAX=50;
var BMIN=0;
var THRESH_MIN=120;
var X_PROBE=200;
var Y_PROBE=196;
var R=0;
var G=0;
var B=0;
var A=0;


colorDetect.onclick = function (event) {
  clearInterval(myTimer);  
  myTimer = setInterval(function(){error_handle();},5000);
  ShowImage.src=location.origin+'/?colorDetect='+Math.random();
}

//ANN:READY
var Module = {
  onRuntimeInitialized(){onOpenCvReady();}
}

function onOpenCvReady(){
  //alert("onOpenCvReady");
  console.log("OpenCV IS READY!!!");
  drawReadyText();  
  document.body.classList.remove("loading");
}

    
function error_handle() {
  restartCount++;
  clearInterval(myTimer);
  if (restartCount<=2) {
    message.innerHTML = "Get still error. <br>Restart ESP32-CAM "+restartCount+" times.";
    myTimer = setInterval(function(){colorDetect.click();},10000);
    ifr.src = document.location.origin+'?restart';
  }
  else
    message.innerHTML = "Get still error. <br>Please close the page and check ESP32-CAM.";
}    
colorDetect.style.display = "block";
ShowImage.onload = function (event) {
  //alert("SHOW IMAGE");
  console.log("SHOW iMAGE");
  clearInterval(myTimer);
  restartCount=0;      
  canvas.setAttribute("width", ShowImage.width);
  canvas.setAttribute("height", ShowImage.height);
  canvas.style.display = "block";
  imageCanvas.setAttribute("width", ShowImage.width);
  imageCanvas.setAttribute("height", ShowImage.height);
  imageCanvas.style.display = "block";

  imageMask.setAttribute("width", ShowImage.width);
  imageMask.setAttribute("height", ShowImage.height);
  imageMask.style.display = "block";      
      
  context.drawImage(ShowImage,0,0,ShowImage.width,ShowImage.height);
  
  DetectImage();        
}
restart.onclick = function (event) {
  fetch(location.origin+'/?restart=stop');
}
quality.onclick = function (event) {
  fetch(document.location.origin+'/?quality='+this.value+';stop');
} 
brightness.onclick = function (event) {
  fetch(document.location.origin+'/?brightness='+this.value+';stop');
} 
contrast.onclick = function (event) {
  fetch(document.location.origin+'/?contrast='+this.value+';stop');
}                             
async function DetectImage() {
  //alert("DETECT IMAGE");
  console.log("DETECT IMAGE");

  /***************opencv********************************/
  //ANN:4
  let src = cv.imread(ShowImage);
  arows = src.rows;
  acols = src.cols;
  aarea = arows*acols;
  adepth = src.depth();
  atype = src.type();
  achannels = src.channels();
  console.log("rows = " + arows);
  console.log("cols = " + acols);
  console.log("pic area = " + aarea);
  console.log("depth = " + adepth); 
  console.log("type = " + atype); 
  console.log("channels = " + achannels);
  
  /******************COLOR DETECT******************************/

  //ANN:6
  var RMAXslider = document.getElementById("rmax");
  var RMAXoutput = document.getElementById("RMAXdemo");
  RMAXoutput.innerHTML = RMAXslider.value;
  RMAXslider.oninput = function(){
  RMAXoutput.innerHTML = this.value;
  RMAX = parseInt(RMAXoutput.innerHTML,10);
  console.log("RMAX=" + RMAX);
  }

  console.log("RMAX=" + RMAX);

  var RMINslider = document.getElementById("rmin");
  var RMINoutput = document.getElementById("RMINdemo");
  RMINoutput.innerHTML = RMINslider.value;
  RMINslider.oninput = function(){
    RMINoutput.innerHTML = this.value;
    RMIN = parseInt(RMINoutput.innerHTML,10);
    console.log("RMIN=" + RMIN);
  }
  console.log("RMIN=" + RMIN);

  var GMAXslider = document.getElementById("gmax");
  var GMAXoutput = document.getElementById("GMAXdemo");
  GMAXoutput.innerHTML = GMAXslider.value;
  GMAXslider.oninput = function(){
    GMAXoutput.innerHTML = this.value;
    GMAX = parseInt(GMAXoutput.innerHTML,10);
  }
  console.log("GMAX=" + GMAX);

  var GMINslider = document.getElementById("gmin");
  var GMINoutput = document.getElementById("GMINdemo");
  GMINoutput.innerHTML = GMINslider.value;
  GMINslider.oninput = function(){
    GMINoutput.innerHTML = this.value;
    GMIN = parseInt(GMINoutput.innerHTML,10);
  }
  console.log("GMIN=" + GMIN);

  var BMAXslider = document.getElementById("bmax");
  var BMAXoutput = document.getElementById("BMAXdemo");
  BMAXoutput.innerHTML = BMAXslider.value;
  BMAXslider.oninput = function(){
    BMAXoutput.innerHTML = this.value;
    BMAX = parseInt(BMAXoutput.innerHTML,10);
  }
  console.log("BMAX=" + BMAX);

  var BMINslider = document.getElementById("bmin");
  var BMINoutput = document.getElementById("BMINdemo");
  BMINoutput.innerHTML = BMINslider.value;
  BMINslider.oninput = function(){
  BMINoutput.innerHTML = this.value;
  BMIN = parseInt(BMINoutput.innerHTML,10);
  }
  console.log("BMIN=" + BMIN);



  var THRESH_MINslider = document.getElementById("thresh_min");
  var THRESH_MINoutput = document.getElementById("THRESH_MINdemo");
  THRESH_MINoutput.innerHTML = THRESH_MINslider.value;
  THRESH_MINslider.oninput = function(){
  THRESH_MINoutput.innerHTML = this.value;
  THRESH_MIN = parseInt(THRESH_MINoutput.innerHTML,10);
  }
  console.log("THRESHOLD MIN=" + THRESH_MIN);

  //ANN:9A
  var X_PROBEslider = document.getElementById("x_probe");
  var X_PROBEoutput = document.getElementById("X_PROBEdemo");
  X_PROBEoutput.innerHTML = X_PROBEslider.value;
  X_PROBEslider.oninput = function(){
  X_PROBEoutput.innerHTML = this.value;
  X_PROBE = parseInt(X_PROBEoutput.innerHTML,10);
  }
  console.log("X_PROBE=" + X_PROBE); 

  var Y_PROBEslider = document.getElementById("y_probe");
  var Y_PROBEoutput = document.getElementById("Y_PROBEdemo");
  Y_PROBEoutput.innerHTML = Y_PROBEslider.value;
  Y_PROBEslider.oninput = function(){
  Y_PROBEoutput.innerHTML = this.value;
  Y_PROBE = parseInt(Y_PROBEoutput.innerHTML,10);
  }
  console.log("Y_PROBE=" + Y_PROBE); 


  document.getElementById('trackButton').onclick = function(){
    b_tracker = (true && !b_tracker)  
    console.log("TRACKER = " + b_tracker );
    var TRACKoutput = document.getElementById("TRACKdemo");
    TRACKoutput.innerHTML = b_tracker;
    //var XCMoutput = document.getElementById("XCMdemo");
    //XCMoutput.innerHTML = x_cm;
 
  }  

  document.getElementById('invertButton').onclick = function(){
    b_invert = (true && !b_invert)  
    console.log("TRACKER = " + b_invert );
    var INVERToutput = document.getElementById("INVERTdemo");
    INVERToutput.innerHTML = b_invert;
  }  
/**/
  document.getElementById('contourButton').onclick = function(){
    b_contour = (true && !b_contour)  
    console.log("TRACKER = " + b_contour );
    var CONTOURoutput = document.getElementById("CONTOURdemo");
    CONTOURoutput.innerHTML = b_contour;
  } 
/**/ 

  let tracker = 0;
  
  var TRACKoutput = document.getElementById("TRACKdemo");
  TRACKoutput.innerHTML = b_tracker;
  var XCMoutput = document.getElementById("XCMdemo");
  var YCMoutput = document.getElementById("YCMdemo");

  XCMoutput.innerHTML = 0;
  YCMoutput.innerHTML = 0; 

  var INVERToutput = document.getElementById("INVERTdemo");
  INVERToutput.innerHTML = b_invert;  

  var CONTOURoutput = document.getElementById("CONTOURdemo");
  CONTOURoutput.innerHTML = b_contour;   

  //ANN:8
  let M00Array = [0,];
  let orig = new cv.Mat();
  let mask = new cv.Mat();
  let mask1 = new cv.Mat();
  let mask2 = new cv.Mat();
  let contours = new cv.MatVector();
  let hierarchy = new cv.Mat();
  let rgbaPlanes = new cv.MatVector();
    
  let color = new cv.Scalar(0,0,0);

  clear_canvas();


    
  orig = cv.imread(ShowImage);
  cv.split(orig,rgbaPlanes);  //SPLIT
  let BP = rgbaPlanes.get(2);  // SELECTED COLOR PLANE
  let GP = rgbaPlanes.get(1);
  let RP = rgbaPlanes.get(0);
  cv.merge(rgbaPlanes,orig);
   
    
              //   BLK    BLU   GRN   RED
  let row = Y_PROBE //180//275 //225 //150 //130    
  let col = X_PROBE //100//10 //100 //200 //300
  drawColRowText(acols,arows);


  console.log("ISCONTINUOUS = " + orig.isContinuous());

  //ANN:9C
  R = src.data[row * src.cols * src.channels() + col * src.channels()];
  G = src.data[row * src.cols * src.channels() + col * src.channels() + 1];
  B = src.data[row * src.cols * src.channels() + col * src.channels() + 2];
  A = src.data[row * src.cols * src.channels() + col * src.channels() + 3];
  console.log("RDATA = " + R);
  console.log("GDATA = " + G);
  console.log("BDATA = " + B);
  console.log("ADATA = " + A);

  drawRGB_PROBE_Text();
  
   
    
  //ANN:9b
  //*************draw probe point*********************
  let point4 = new cv.Point(col,row);
  cv.circle(src,point4,5,[255,255,255,255],2,cv.LINE_AA,0);
  //***********end draw probe point*********************

  //ANN:7
  let high = new cv.Mat(src.rows,src.cols,src.type(),[RMAX,GMAX,BMAX,255]);
  let low = new cv.Mat(src.rows,src.cols,src.type(),[RMIN,GMIN,BMIN,0]);

  cv.inRange(src,low,high,mask1);
  //inRange(source image, lower limit, higher limit, destination image)
    
  cv.threshold(mask1,mask,THRESH_MIN,255,cv.THRESH_BINARY);
  //threshold(source image,destination image,threshold,255,threshold method);

  //ANN:9
  if(b_invert==true){
     cv.bitwise_not(mask,mask2);
  }
/********************start contours******************************************/
  //ANN:10
  if(b_tracker == true){
  try{
   if(b_invert==false){
    //ANN:11   
    cv.findContours(mask,contours,hierarchy,cv.RETR_CCOMP,cv.CHAIN_APPROX_SIMPLE);
    //findContours(source image, array of contours found, hierarchy of contours
        // if contours are inside other contours, method of contour data retrieval,
        //algorithm method)
   }
   else{
    cv.findContours(mask2,contours,hierarchy,cv.RETR_CCOMP,cv.CHAIN_APPROX_SIMPLE);
   }
    console.log("CONTOUR_SIZE = " + contours.size());

    //draw contours
    if(b_contour==true){
     for(let i = 0; i < contours.size(); i++){
        cv.drawContours(src,contours,i,[0,0,0,255],2,cv.LINE_8,hierarchy,100)
     }
    }

    //ANN:12
    let cnt;
    let Moments;
    let M00;
    let M10;
    //let x_cm;
    //let y_cm;
    
    //ANN:13
    for(let k = 0; k < contours.size(); k++){
        cnt = contours.get(k); 
        Moments = cv.moments(cnt,false);
        M00Array[k] = Moments.m00;
       // cnt.delete();
    }

    //ANN13A
    let max_area_arg = MaxAreaArg(M00Array);
    console.log("MAXAREAARG = "+max_area_arg);

    //let TestArray = [0,0,0,15,4,15,2];
    //let TestArray0 = [];
    //let max_test_area_arg = MaxAreaArg(TestArray0);
    //console.log("MAXTESTAREAARG = "+max_test_area_arg);



    let ArgMaxArea = MaxAreaArg(M00Array);
    if(ArgMaxArea >= 0){
    cnt = contours.get(MaxAreaArg(M00Array));  //use the contour with biggest MOO
    //cnt = contours.get(54);
    Moments = cv.moments(cnt,false);
    M00 = Moments.m00;
    M10 = Moments.m10;
    M01 = Moments.m01;
    x_cm = M10/M00;    // 75 for circle_9.jpg
    y_cm = M01/M00;    // 41 for circle_9.jpg

    XCMoutput.innerHTML = Math.round(x_cm);
    YCMoutput.innerHTML = Math.round(y_cm);

    console.log("M00 = "+M00);  
    console.log("XCM = "+Math.round(x_cm));
    console.log("YCM = "+Math.round(y_cm)); 

    //fetch(document.location.origin+'/?xcm='+Math.round(x_cm)+';stop');
    fetch(document.location.origin+'/?cm='+Math.round(x_cm)+';'+Math.round(y_cm)+';stop');

    console.log("M00ARRAY = " + M00Array);

    //ANN:14   
    
    //**************min area bounding rect********************
    //let rotatedRect=cv.minAreaRect(cnt);
    //let vertices = cv.RotatedRect.points(rotatedRect);

    //for(let j=0;j<4;j++){
    //    cv.line(src,vertices[j],
    //        vertices[(j+1)%4],[0,0,255,255],2,cv.LINE_AA,0);
    //}
    //***************end min area bounding rect*************************************


    //***************bounding rect***************************
    let rect = cv.boundingRect(cnt);
    let point1 = new cv.Point(rect.x,rect.y);
    let point2 = new cv.Point(rect.x+rect.width,rect.y+rect.height);

    cv.rectangle(src,point1,point2,[0,0,255,255],2,cv.LINE_AA,0);
    //*************end bounding rect***************************


    //*************draw center point*********************
    let point3 = new cv.Point(x_cm,y_cm);
    cv.circle(src,point3,2,[0,0,255,255],2,cv.LINE_AA,0);
    //***********end draw center point*********************

    }//end if(ArgMaxArea >= 0)
    else{
      if(ArgMaxArea==-1){ 
        console.log("ZERO ARRAY LENGTH");
      }
      else{              //ArgMaxArea=-2
        console.log("DUPLICATE MAX ARRAY-ELEMENT");
      }
    }




    cnt.delete();
/******************end contours  note cnt line one up*******************************************/
   drawXCM_YCM_Text();

  }//end try
  catch{
    console.log("ERROR TRACKER NO CONTOUR");
    clear_canvas();
    drawErrorTracking_Text();
  }
    
  }//end b_tracking if statement
  else{
      XCMoutput.innerHTML = 0;
      YCMoutput.innerHTML = 0;
  }    

  if(b_invert==false){
     cv.imshow('imageMask', mask);
  }
  else{
     cv.imshow('imageMask', mask2);
  }
  //cv.imshow('imageMask', R);
  cv.imshow('imageCanvas', src);

  //ANN:8A
  src.delete();
  high.delete();
  low.delete();
  orig.delete();
  mask1.delete();
  mask2.delete();
  mask.delete();
  contours.delete();
  hierarchy.delete();
  //cnt.delete();
  RP.delete();
    
  


 /********************END COLOR DETECT****************************/
  
/***************end opencv******************************/
      

 setTimeout(function(){colorDetect.click();},500);
  
}//end detectimage 


function MaxAreaArg(arr){
    if (arr.length == 0) {
        return -1;
    }

    var max = arr[0];
    var maxIndex = 0;
    var dupIndexCount = 0; //duplicate max elements?

    if(arr[0] >= .90*aarea){
        max = 0;
    }

    for (var i = 1; i < arr.length; i++) {
        if (arr[i] > max && arr[i] < .99*aarea) {
            maxIndex = i;
            max = arr[i];
            dupIndexCount = 0;
        }
        else if(arr[i]==max && arr[i]!=0){
            dupIndexCount++;
        }
    }

    if(dupIndexCount==0){
        return maxIndex;
    }

    else{
        return -2;
    }        
}//end MaxAreaArg    



function clear_canvas(){
    ctx.clearRect(0,0,txtcanvas.width,txtcanvas.height);
    ctx.rect(0,0,txtcanvas.width,txtcanvas.height);
    ctx.fillStyle="red";
    ctx.fill();
}

function drawReadyText(){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('OpenCV.JS READY',txtcanvas.width/4,txtcanvas.height/10);
}          

function drawColRowText(x,y){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('ImageCols='+x,0,txtcanvas.height/10);
    ctx.fillText('ImageRows='+y,txtcanvas.width/2,txtcanvas.height/10);
} 

function drawRGB_PROBE_Text(){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('Rp='+R,0,2*txtcanvas.height/10);
    ctx.fillText('Gp='+G,txtcanvas.width/4,2*txtcanvas.height/10);
    ctx.fillText('Bp='+B,txtcanvas.width/2,2*txtcanvas.height/10);
    ctx.fillText('Ap='+A,3*txtcanvas.width/4,2*txtcanvas.height/10);
}

function drawXCM_YCM_Text(){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('XCM='+Math.round(x_cm),0,3*txtcanvas.height/10); 
    ctx.fillText('YCM='+Math.round(y_cm),txtcanvas.width/4,3*txtcanvas.height/10);    
}

function drawErrorTracking_Text(){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('ERROR TRACKING-NO CONTOUR',0,3*txtcanvas.height/10);
}          
         
  </script> 
</body>
</html>  
)rawliteral";
