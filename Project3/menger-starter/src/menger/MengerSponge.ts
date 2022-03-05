import { Mat3, Mat4, Vec3, Vec4 } from "../lib/TSM.js";

/* A potential interface that students should implement */
interface IMengerSponge {
  setLevel(level: number): void;
  isDirty(): boolean;
  setClean(): void;
  normalsFlat(): Float32Array;
  indicesFlat(): Uint32Array;
  positionsFlat(): Float32Array;
}

/**
 * Represents a Menger Sponge
 */
export class MengerSponge implements IMengerSponge {

  // TODO: sponge data structures
   nesting_level:number = 0
	 dirty:boolean = false
   obj_vertices = new Float32Array();
   vtx_normals = new Float32Array();
   obj_faces = new Uint32Array();
  
  constructor(level: number) {
	  this.setLevel(level);
	  // TODO: other initialization	
  }

  /**
   * Returns true if the sponge has changed.
   */
  public isDirty(): boolean {
       return this.dirty;
  }

  public setClean(): void {
    this.dirty = false
  } 
  
  public setLevel(level: number)
  {
    this.nesting_level = level
    this.dirty = true
  }

  /* Returns a flat Float32Array of the sponge's vertex positions */
  public positionsFlat(): Float32Array {

    

	  // TODO: right now this makes a single triangle. Make the cube fractal instead.
	  return new Float32Array([1.0, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0]);
  }

  public set(): void {
    this.obj_vertices = new Float32Array();
    this.vtx_normals = new Float32Array();
    this.obj_faces = new Uint32Array();

    var minx, miny, minz, maxx, box_length;
    minx = -0.5
	  miny = -0.5
	  minz = -0.5
	  maxx = 0.5
    box_length = maxx - minx
    if (this.nesting_level == 0) {
      this.setObjectVerticiesFacesNormals(minx, miny, minz, minx + box_length, miny + box_length, minz + box_length, 0)
    }else{
      this.createSmallCubes(minx, miny, minz, box_length /= 3.0, this.nesting_level, 0)
    }
  }

  private createSmallCubes(minX, minY, minZ, boxLenght, currentDepth, start): number {
    var x, y, z;
    for (var i = 0; i < 3; i++){
      for (var j = 0; j < 3; j++){
        for (var k = 0; k < 3; k++){
          if (i % 2 + j % 2 + k % 2 < 2){
            x = minX + i * boxLenght
            y = minY + j * boxLenght
            z = minZ + k * boxLenght

            if (currentDepth > 1){
              var new_lenght = boxLenght / 3.0
              start = this.createSmallCubes(x, y, z, new_lenght, currentDepth - 1, start)
            }else{
              this.setObjectVerticiesFacesNormals(x, y, z, x + boxLenght, y + boxLenght, z + boxLenght, start)
              start += 36
            }
          }
        }
      }
    }
    return start
  }

  //Draws a single cube given six vertices
  private setObjectVerticiesFacesNormals(minX, minY, minZ, maxX, maxY, maxZ, start): void {
    var obj_vertices_arr:number[] = []
    var obj_normals_arr:number[] = []
    var obj_faces_arr:number[] = []

    //Triangle 1, front facing, bottom right

    //Triangle 2, front facing, top left

    //Triangle 3, left facing, bottom right

    //Triangle 4, left facing, top left

    //Triangle 5, back facing, bottom right

    //Triangle 6, back facing, top left

    //Triangle 7, right facing, bottom right

    //Triangle 8, right facing, top left

    //Triangle 9, top facing, bottom right

    //Triangle 10, top facing, top left

    //Triangle 11, bottom facing, bottom right

    //Triangle 12, bottom facing, top left

  }

  /**
   * Returns a flat Uint32Array of the sponge's face indices
   */
  public indicesFlat(): Uint32Array {
    // TODO: right now this makes a single triangle. Make the cube fractal instead.
    return new Uint32Array([0, 1, 2]);
  }

  /**
   * Returns a flat Float32Array of the sponge's normals
   */
  public normalsFlat(): Float32Array {
	  // TODO: right now this makes a single triangle. Make the cube fractal instead.
	  return new Float32Array([0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0]);
  }

  /**
   * Returns the model matrix of the sponge
   */
  public uMatrix(): Mat4 {

    // TODO: change this, if it's useful
    const ret : Mat4 = new Mat4().setIdentity();

    return ret;    
  }
  
}
