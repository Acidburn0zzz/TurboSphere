RequireSystemScript('turbo/format.js');
RequireSystemScript('turbo/tileset.js');
RequireSystemScript('turbo/person.js');
RequireSystemScript('turbo/bytearray.js');
RequireSystemScript('turbo/for_each.js');

if(typeof Turbo == "undefined")
    var Turbo = {};
if(typeof Turbo == "undefined")
    Turbo.Classic = {};

Turbo.LoadMapFile = function(path){
    var map_file = new RawFile("../maps/"+path);
    var map_buffer = map_file.read(map_file.size);

    var map_byteview = new Uint8Array(map_buffer);
    var map_bytearray = ByteArrayFromTypedArray(map_byteview);

    return new Turbo.Map(map_bytearray);
}

Turbo.Map = function(bytearray, offset, compat){

    if(typeof offset == "undefined")
      offset = 0;

    if(typeof compat == "undefined")
      compat = true;
/*
    this.signature = ".rmp";

    if(bytearray.length<0x100) // Header plus the first string byte.
      throw "Argument 0 is to short to be a map. Or a stormtrooper.";

    var sig_array = bytearray.slice(0x00, 0x04);
    var sig_string = CreateStringFromByteArray(sig_array);

    if(sig_string!=this.signature)
      throw "Signature is invalid. Expected " + this.signature + ", found " + sig_string;

    var at = 0x04;


    this.render_script = function(){}
    this.update_script = function(){}


    this.version = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.type    = bytearray[at++];
    this.layers  = new Array(bytearray[at++]);
    at++; //reserved
    this.entities= new Array(Turbo.dByteCat(bytearray[at++], bytearray[at++]));

    this.startx  = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.starty  = Turbo.dByteCat(bytearray[at++], bytearray[at++]);
    this.start_layer     = bytearray[at++];
    this.start_direction = bytearray[at++];
    this.strings = new Array(Turbo.dByteCat(bytearray[at++], bytearray[at++]));
    this.functions = new Array(this.strings.length-3);
    this.zones   = new Array(bytearray[at++]);
    this.shader = GetDefaultShaderProgram();

    this.default_scripts = new Array(6);
    this.default_scripts.forEach(function(i){i = function(){}});
*/


    this.__proto__ = Turbo.ReadBinaryObject(bytearray, offset, Turbo.MapScheme.header);
    var at = offset + Turbo.GetSchemeLength(Turbo.TilesetScheme.header);

    this.fps = 60;

    // This is the actual blitting coordinates for the upper-left most part of the screen.
    //  The results of all camera location calculations goes in here.
    this.camera = {x:0, y:0};

    this.unsetCamera = function(){
        this.camera_person = null;
    }

    this.unsetInput = function(){
        this.input_person = null;
    }

    this.unsetCamera();
    this.unsetInput();

    while(at<0x100){
        if(bytearray[++at]!=0){
            throw "Expected clear data at header offset " + at;
        }
    }

    at = 0x100;

    for(var i = 0; i< this.strings.length; i++){
        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        this.strings[i] = string.string;
    }

    for(var i = 0; i< this.functions.length; i++){
        this.functions[i] = function(){eval(this.strings[i+3]);};
    }



    for(var i = 0; i< this.layers.length; i++){

        var from = at;

        if(bytearray.length<at+29) // Up to the first byte of the name string.
          throw "Unexpected end of file.";

        var size = {w:Turbo.dByteCat(bytearray[at++], bytearray[at++]), h:Turbo.dByteCat(bytearray[at++], bytearray[at++])};
        var flags= Turbo.dByteCat(bytearray[at++], bytearray[at++])

        var segmentbuffer = new Float32Array(bytearray.buffer.slice(at, at+16));

        var parallax = {
            x:segmentbuffer[0],
            y:segmentbuffer[1]
        };
        var scrolling= {
            x:segmentbuffer[2],
            y:segmentbuffer[3]
        };

        at+=16;

        var segments = new Array(Turbo.qByteCat(bytearray[at++], bytearray[at++], bytearray[at++], bytearray[at++]));

        /*reflective, reserved[3]*/ at+=4;


        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        var name = string.string;

        var data = new Uint16Array(bytearray, at, size.w*size.h*2);

        at+=size.w*size.h*2;

        for(var e in segments){

            if(bytearray.length<at+8)
              throw "Unexpected end of file.";

            var segmentbuffer = new Uint32Array(bytearray.buffer.slice(at, at+16));

            segments[e] = [{x:segmentbuffer[0], y:segmentbuffer[1]},
                           {x:segmentbuffer[2], y:segmentbuffer[3]}];

            at+=16;
        }

        this.layers[i] = {size:size, flags:flags, parallax:parallax,
            scrolling:scrolling, name:name, segments:segments, field:data,
            surface:new Surface(this.width, this.height, new Color(0,0,0,0)),
            shader:this.shader, visibile:true, reflective:false, mask:new Color(0xFF, 0xFF, 0xFF)};

    }

    for(var i = 0; i< this.entities.length; i++){
        at += Turbo.LoadEntity(bytearray, at, this.entities[i]);
    }

    for(var i = 0; i<this.zones.length; i++){

        if(bytearray.length < at+12+4)
          throw "Unexpected end of file.";

        var zonebuffer = new Uint16Array(bytearray.buffer.slice(at, at+12));

        this.zones[i] = {location:[{x:zonebuffer[0],
                               y:zonebuffer[1]},
                              {x:zonebuffer[2],
                               y:zonebuffer[3]}],
                               layer:zonebuffer[4],
                               steps:zonebuffer[5]};

        /*Reserved[4], and the twelve we pulled from the zonebuffer.*/at+=4+12;

        var string = Turbo.Classic.readString(bytearray, at);
        at+=string.length;
        this.zones[i].on_touch = function(){eval(string.string);};
        this.zones[i].cursor = 0;

        this.zones[i].walk = function(){
            this.cursor++;
            if(this.cursor==this.steps){
                this.cursor=0;
                this.current_zone = this.zones[i];
                this.on_touch();
            }
        }

        this.zones[i].tryWalk = function(_x, _y){
            if((_x>this.location[0].x) &&
               (_x<this.location[1].x) &&
               (_y>this.location[0].y) &&
               (_y<this.location[1].y)){
                this.walk();
            }
        }
    }

    if(this.strings[0]){
        this.tileset = Turbo.LoadTilesetFile(strings[0]);
    }
    else{
        this.tileset = new Turbo.Tileset(bytearray, at);
    }

    this.calculateLayer = function(i){
        var shapes = [];

        for(var x = 0; x<this.width; x++){
            for(var y = 0; y<this.height; y++){
                at = x+(y*this.width);

                shapes.push(new Shape([
                    {x:(x+0)*this.width, y:(y+0)*this.height},
                    {x:(x+1)*this.width, y:(y+0)*this.height},
                    {x:(x+1)*this.width, y:(y+1)*this.height},
                    {x:(x+0)*this.width, y:(y+1)*this.height}],
                  this.tileset.tiles[this.layers[i].field[at]].image)
                );

            }
        }

        this.layers[i].group = new Group(shapes, this.layers[i].shader);

    }

    this.calculateMap = function(){
        for(var i in this.layers){
            this.calculateLayer(i);
        }
    }

    this.calculateCamera = function(){
        this.camera.x = camera_person.x;
        this.camera.y = camera_person.y;
    }

    this.drawLayer = function(i){
        this.calculateCamera();
        this.layer[i].group.x = this.camera.x;
        this.layer[i].group.y = this.camera.y;
        this.layer[i].group.draw();
    }

}


