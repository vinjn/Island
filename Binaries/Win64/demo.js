printi(134);
let actor = SpawnActor();
let box = CreateBoxComponent(actor);
SetActorLocation(actor, 0, 0, 400);

let pong = 13;
printi(13);
SetWindowTitle("demo.js");
// let asset = glTFLoadAssetFromFilename("c:/svn_pool/UnityGLTF/UnityGLTF/www/glTF-Sample-Models/2.0/VC/glTF-Embedded/VC.gltf");
let asset = glTFLoadAssetFromFilename("d:/cricket-frog-fejervarya-kawamurai/source/Q11275-1all.gltf");
let gltfActor = gltfSpawnActor(asset);
SetActorLocation(gltfActor, 0, 0, 300);
// SetSimulatePhysics(gltfActor, true);