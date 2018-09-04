/* Signed Distance Fields Generator.
	Generating signed distance fields based on 8SSEDT and 4SSEDT.
	References:
	- Leymarie, F. & Levine, M.D.; A Note on "Fast Raster Scan Distance Propagation on the Discrete Rectangular Lattice"
	- http://www.codersnotes.com/notes/signed-distance-fields/
*/
#ifndef __SDF_GENERATOR_H__
#define __SDF_GENERATOR_H__

#define SDF_GEN_VERSION 1

#define _8SSEDT 1
#define _4SSEDT 2

#ifndef SDF_METHOD
#define SDF_METHOD _8SSEDT
#endif

typedef unsigned int sdf_uint;
typedef unsigned char sdf_uchar;

#ifdef __cplusplus
extern "C" {
#endif

	// Generate Signed Distance from image buffer;
	// outputBuffer must be allocated width*height;
	void generate_sdf_from_image(sdf_uchar* imageBuffer, sdf_uint width, sdf_uint height, sdf_uchar* ouputBuffer);

#ifdef __cplusplus
}
#endif

#endif