SetWindowTitle("demo.js");

let testPrint = function() {
    printi(134);
    let pong = 13;
    printi(pong);
}

let testActor = function() {
    let actor = SpawnActor();
    let box = CreateBoxComponent(actor);
    SetActorLocation(actor, 0, 0, 400);
}

let gltfActor;
let testGltf = function() {
    // let asset = glTFLoadAssetFromFilename("c:/svn_pool/UnityGLTF/UnityGLTF/www/glTF-Sample-Models/2.0/VC/glTF-Embedded/VC.gltf");
    let asset = glTFLoadAssetFromFilename("d:/cricket-frog-fejervarya-kawamurai/source/Q11275-1all.gltf");
    gltfActor = gltfSpawnActor(asset);
    SetActorLocation(gltfActor, 0, 0, 300);
    // SetSimulatePhysics(gltfActor, true);
}

let update = function() {
    prints("update");
    SetActorLocation(gltfActor, 0, 0, 300);
}
