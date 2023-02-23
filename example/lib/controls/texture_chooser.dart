import 'package:flutter/material.dart';
import 'package:flutter_opengl/flutter_opengl.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:star_menu/star_menu.dart';

import '../states.dart';

/// Row of 4 TextureWidget that represent the 4 iChannel[0-3]
///
class TextureChooser extends ConsumerWidget {
  const TextureChooser({
    Key? key,
  }) : super(key: key);

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    int shaderIndex = ref.watch(stateShaderIndex);
    if (shaderIndex == -1) {
      return const SizedBox.shrink();
    }

    return Row(
      mainAxisAlignment: MainAxisAlignment.center,
      children: List.generate(4, (index) => TextureWidget(channelId: index)),
    );
  }
}

/// Widget that display the current binded texture
///
class TextureWidget extends ConsumerWidget {
  final int channelId;
  final double? width;
  final double? height;

  const TextureWidget({
    Key? key,
    required this.channelId,
    this.width = 80,
    this.height = 80,
  }) : super(key: key);

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final TextureParams texture;
    switch (channelId) {
      case 0:
        texture = ref.watch(stateChannel0);
        break;
      case 1:
        texture = ref.watch(stateChannel1);
        break;
      case 2:
        texture = ref.watch(stateChannel2);
        break;
      case 3:
        texture = ref.watch(stateChannel3);
        break;
      default:
        texture = ref.watch(stateChannel0);
    }

    return Column(
      children: [
        Stack(
          children: [
            /// STARMENU POPUP
            StarMenu(
              params: StarMenuParameters(
                  backgroundParams: const BackgroundParams(
                      animatedBlur: true, sigmaX: 12, sigmaY: 12),
                  shape: MenuShape.linear,
                  linearShapeParams: const LinearShapeParams(
                    alignment: LinearAlignment.center,
                  ),
                  boundaryBackground: BoundaryBackground(
                    color: const Color(0x800e0e0e),
                  ),
                  centerOffset: const Offset(0, -100)),
              items: _items(ref),
              onItemTapped: (index, controller) {
                controller.closeMenu!();
              },
              child: Container(
                width: width,
                height: height,
                margin: const EdgeInsets.all(6),
                decoration: BoxDecoration(
                  color: Colors.black,
                  borderRadius: const BorderRadius.all(Radius.circular(10)),
                  border: Border.all(width: 3, color: Colors.white),
                  image: texture.assetImage.isEmpty
                      ? null
                      : DecorationImage(
                          fit: BoxFit.cover,
                          image: AssetImage(texture.assetImage),
                        ),
                ),
              ),
            ),

            /// REMOVE TEXTURE
            Positioned(
              right: 9,
              top: 9,
              child: GestureDetector(
                onTap: () {
                  bool removed = OpenGLController()
                      .openglFFI
                      .removeUniform('iChannel$channelId');
                  if (removed) {
                    StateController<TextureParams> channelProvider;
                    _clearTexture(ref);
                  }
                },
                child: const Icon(Icons.delete_outline, size: 24),
              ),
            ),

            /// STOP CAPTURE
            /// since only once instance of capture is available for now
            /// this is visible on all [TextureChooser]
            Visibility(
              visible: ref.watch(stateCaptureRunning),
              child: Positioned(
                left: 9,
                bottom: 9,
                child: GestureDetector(
                  onTap: () {
                    bool ret = OpenGLController().openglFFI.stopCapture();
                    ref.read(stateCaptureRunning.notifier).state = false;
                  },
                  child: const Icon(Icons.stop_circle, size: 24),
                ),
              ),
            ),
          ],
        ),
        Text('iChannel$channelId'),
      ],
    );
  }

  _clearTexture(WidgetRef ref) {
    switch (channelId) {
      case 0:
        ref.read(stateChannel0.notifier).state =
            TextureParams().copyWith(assetsImage: '');
        break;
      case 1:
        ref.read(stateChannel1.notifier).state =
            TextureParams().copyWith(assetsImage: '');
        break;
      case 2:
        ref.read(stateChannel2.notifier).state =
            TextureParams().copyWith(assetsImage: '');
        break;
      case 3:
        ref.read(stateChannel3.notifier).state =
            TextureParams().copyWith(assetsImage: '');
        break;
    }
  }

  List<Widget> _items(WidgetRef ref) {
    return [
      Item(
          channelId: channelId,
          assetImage: 'assets/dash.png',
          text: '1481x900'),
      Item(
          channelId: channelId,
          assetImage: 'assets/flutter.png',
          text: '512x512'),
      Item(
          channelId: channelId,
          assetImage: 'assets/rgba-noise-medium.png',
          text: '96x96'),
      Item(
          channelId: channelId,
          assetImage: 'assets/rgba-noise-small.png',
          text: '96x96'),
      Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          SizedBox(
            width: 64,
            height: 64,
            child: IconButton(
              onPressed: () {
                bool ret = OpenGLController().openglFFI.startCaptureOnSampler2D(
                    'iChannel$channelId', ref.read(statePickedVideo));
                ref.read(stateCaptureRunning.notifier).state = true;
                _clearTexture(ref);
              },
              icon: const Icon(Icons.ondemand_video_outlined, size: 64),
            ),
          ),
          const SizedBox(width: 30),
          SizedBox(
            width: 64,
            height: 64,
            child: IconButton(
              onPressed: () {
                bool ret = OpenGLController()
                    .openglFFI
                    .startCaptureOnSampler2D('iChannel$channelId', 'cam0');
                ref.read(stateCaptureRunning.notifier).state = ret;
                _clearTexture(ref);
              },
              icon: const Icon(Icons.camera, size: 64),
            ),
          ),
        ],
      ),
    ];
  }
}

/// Entry for the menu
///
/// It display the texture image and its resolution
class Item extends ConsumerWidget {
  final int channelId;
  final String assetImage;
  final String text;

  const Item({
    Key? key,
    required this.channelId,
    required this.assetImage,
    required this.text,
  }) : super(key: key);

  _setTexture(AddMethod method, WidgetRef ref) {
    var channelProvider;
    switch (channelId) {
      case 0:
        channelProvider = ref.read(stateChannel0.notifier);
        break;
      case 1:
        channelProvider = ref.read(stateChannel1.notifier);
        break;
      case 2:
        channelProvider = ref.read(stateChannel2.notifier);
        break;
      case 3:
        channelProvider = ref.read(stateChannel3.notifier);
        break;
      default:
        channelProvider = ref.read(stateChannel0.notifier);
    }

    OGLUtils.setAssetTexture('iChannel$channelId', assetImage, method: method)
        .then((value) {
      if (value) {
        channelProvider.state =
            TextureParams().copyWith(assetsImage: assetImage);
      }
    });
  }

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: Container(
        decoration: BoxDecoration(
          color: const Color(0xFF3f3f3f),
          borderRadius: const BorderRadius.all(Radius.circular(10)),
          border: Border.all(width: 1, color: Colors.black),
        ),
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Padding(
              padding: const EdgeInsets.all(2.0),
              child: Image.asset(assetImage, width: 80, height: 80),
            ),
            const SizedBox(width: 10),
            Text(text),
            const SizedBox(width: 10),
            IconButton(
              icon: const Icon(Icons.add),
              tooltip: 'add new',
              onPressed: () {
                _setTexture(AddMethod.add, ref);
              },
            ),
            IconButton(
              icon: const Icon(Icons.find_replace),
              tooltip: 'replace different size',
              onPressed: () {
                _setTexture(AddMethod.replace, ref);
              },
            ),
            IconButton(
              icon: const Icon(Icons.settings_overscan_outlined),
              tooltip: 'replace same size',
              onPressed: () {
                _setTexture(AddMethod.set, ref);
              },
            ),
          ],
        ),
      ),
    );
  }
}
