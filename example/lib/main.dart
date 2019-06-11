import 'package:flutter/material.dart';

import 'package:opengl/opengl.dart';

void main() => runApp(MyApp());

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => new _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String notes;
  final controller = Opengl();
  final _width = 300.0;
  final _height = 300.0;

  @override
  initState() {
    super.initState();

    notes="";
    initializeNDKController();
  }

  @override
  void dispose() {
    stopNDKController();
    super.dispose();
  }


  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: Text('OpenGL plugin example'),
        ),
        body: new Center(
          child: Column(
            children: <Widget>[
              SizedBox(height: 8.0,),
              Container(
                width: _width,
                height: _height,
                child: controller.isInitialized
                    ? Texture(textureId: controller.textureId)
                    : Container(),
              ),
              Padding(
                padding: const EdgeInsets.only(top: 16.0),
                child: Text(notes),
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  RaisedButton(
                    child: Text("stop"),
                    color: Colors.red,
                    textColor: Colors.white,
                    onPressed: () {
                      stopNDKController();
                    },
                  ),
                  SizedBox(width: 20),
                  RaisedButton(
                    child: Text("re-create"),
                    color: Colors.redAccent,
                    textColor: Colors.white,
                    onPressed: () {
                      initializeNDKController();
                    },
                  ),
                ],
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  RaisedButton(
                    child: Text("OpenGL ES 1\nfunc 1"),
                    onPressed: () {
                      setNDKRenderFuncController(0);
                      notes = "";
                      setState(() {});
                    },
                  ),
                  SizedBox(width: 6),
                  RaisedButton(
                    child: Text("OpenGL ES 2\nshader 1"),
                    onPressed: () {
                      setNDKRenderFuncController(1);
                      notes = "COMPUTATIONAL INTENSIVE\nhttps://www.shadertoy.com/view/3l23Rh";
                      setState(() {});
                    },
                  ),
                ],
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  RaisedButton(
                    child: Text("OpenGL ES 2\nshader 2"),
                    onPressed: () {
                      setNDKRenderFuncController(2);
                      notes = "https://www.shadertoy.com/view/XsXXDn";
                      setState(() {});
                    },
                  ),
                  SizedBox(width: 6),
                  RaisedButton(
                    child: Text("OpenGL ES 2\nshader 3"),
                    onPressed: () {
                      setNDKRenderFuncController(3);
                      notes = "https://www.shadertoy.com/view/ttlGDf";
                      setState(() {});
                    },
                  ),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }

  initializeNDKController() async {
    await controller.initializeNDK(0, _width, _height);
    setState(() {});
  }

  stopNDKController() async {
    await controller.stopNDK();
    setState(() {});
  }

  setNDKRenderFuncController(int func) async {
    await controller.stopNDK();
    await controller.initializeNDK(func, _width, _height);
    setState(() {});
  }
}
