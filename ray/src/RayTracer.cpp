// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "scene/kdTree.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <string.h> // for memset

#include <iostream>
#include <fstream>


using namespace std;
extern TraceUI* traceUI;
class Node;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through pixel(i,j), i.e. normalized window coordinates (x,y),
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.

glm::dvec3 RayTracer::trace(double x, double y)
{
        // Clear out the ray cache in the scene for debugging purposes,
        if (TraceUI::m_debug)
        {
                scene->clearIntersectCache();
        }
        
        ray r(glm::dvec3(0,0,0), glm::dvec3(0,0,0), glm::dvec3(1,1,1), ray::VISIBILITY);
        scene->getCamera().rayThrough(x,y,r);
        double dummy;
        glm::dvec3 ret = traceRay(r, glm::dvec3(1.0,1.0,1.0), traceUI->getDepth(), dummy);
        ret = glm::clamp(ret, 0.0, 1.0);
        return ret;
}

glm::dvec3 RayTracer::tracePixel(int i, int j)
{
        glm::dvec3 col(0,0,0);

        if( ! sceneLoaded() ) return col;
       double x = double(i)/double(buffer_width);
       double y = double(j)/double(buffer_height);
       unsigned char *pixel = buffer.data() + ( i + j * buffer_width ) * 3;
 
       double x_incr = (1.0/double(buffer_width))/samples;
       double y_incr = (1.0/double(buffer_height))/samples;
       for (double w = 0; w < samples; w++) {
               for (double h = 0; h < samples; h++) {
                       col += trace(x + w*x_incr, y + h*y_incr);
               }
       }
       col /= (double) (samples * samples);
       //cout << samples << endl;
       pixel[0] = (int)( 255.0 * col[0]);
       pixel[1] = (int)( 255.0 * col[1]);
       pixel[2] = (int)( 255.0 * col[2]);
       return col;
}

#define VERBOSE 0

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
glm::dvec3 RayTracer::traceRay(ray& r, const glm::dvec3& thresh, int depth, double& t )
{
        isect i;

        glm::dvec3 colorC;
#if VERBOSE
        std::cerr << "== current depth: " << depth << std::endl;
#endif

        if(scene->intersect(r, i)) {//r I and surface orurred!  We've got work to do.  For now,
                // this code gets the material for the surface that was intersected,
                // and asks that material to provide a color for the ray.

                // This is a great place to insert code for recursive ray tracing.
                // Instead of just returning the result of shade(), add some
                // more steps: add in the contributions from reflected and refracted
                // rays.		
				//Q is intersection of r with the surface
				glm::dvec3 Q = r.at(i.getT());
                                const Material& m = i.getMaterial();
                                colorC = m.shade(scene.get(), r, i);
                                //recursion check
                                if (depth <= 0) {
                                        return colorC;
                                }
                                //Calculate reflection unit vector and ray
                                glm::dvec3 ref = glm::normalize(glm::reflect(r.getDirection(), i.getN()));
                                ray REF_RAY(Q,ref, glm::dvec3(1,1,1), ray::REFLECTION);
                                glm::dvec3 k_r = m.kr(i);
                                colorC = colorC + k_r * traceRay(REF_RAY, thresh, depth - 1, t);
                                double n_i;
                                double n_t;
                                if (glm::dot(r.getDirection(), i.getN()) > 0.0 ) {//entering object
                                        n_i = 1.0;
                                        n_t = m.index(i);
                                } else { //exiting object
                                        n_i = m.index(i);
                                        n_t = 1.0;                                        
                                }
                                double neta = (n_i/n_t);
                                //refraction vector calculation
                                glm::dvec3 refract = glm::normalize(glm::refract(-1.0 * r.getDirection(), i.getN(), neta));
                                glm::dvec3 zero(0.0, 0.0, 0.0);
                                //check that kt(i) is not the zero vector and TIR did not occur
                                if (glm::greaterThan(m.kt(i), zero)[0] && !glm::all(glm::isnan(refract))) {
                                        ray REFRACT(Q,refract, glm::dvec3(1,1,1), ray::REFRACTION);
                                        colorC += m.kt(i) * traceRay(REFRACT, thresh, depth - 1, t);
                                }
        } else {
                // No intersection.  This ray travels to infinity, so we color
                // it according to the background color, which in this (simple) case
                // is just black.
                //
                // FIXME: Add CubeMap support here.
                // TIPS: CubeMap object can be fetched from traceUI->getCubeMap();
                //       Check traceUI->cubeMap() to see if cubeMap is loaded
                //       and enabled.

                colorC = glm::dvec3(0.0, 0.0, 0.0);
        }
#if VERBOSE
        std::cerr << "== depth: " << depth+1 << " done, returning: " << colorC << std::endl;
#endif
        return colorC;
}

RayTracer::RayTracer()
        : scene(nullptr), buffer(0), thresh(0), buffer_width(0), buffer_height(0), m_bBufferReady(false)
{
}

RayTracer::~RayTracer()
{
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
        buf = buffer.data();
        w = buffer_width;
        h = buffer_height;
}

double RayTracer::aspectRatio()
{
        return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene(const char* fn)
{
        ifstream ifs(fn);
        if( !ifs ) {
                string msg( "Error: couldn't read scene file " );
                msg.append( fn );
                traceUI->alert( msg );
                return false;
        }

        // Strip off filename, leaving only the path:
        string path( fn );
        if (path.find_last_of( "\\/" ) == string::npos)
                path = ".";
        else
                path = path.substr(0, path.find_last_of( "\\/" ));

        // Call this with 'true' for debug output from the tokenizer
        Tokenizer tokenizer( ifs, false );
        Parser parser( tokenizer, path );
        try {
                scene.reset(parser.parseScene());
        }
        catch( SyntaxErrorException& pe ) {
                traceUI->alert( pe.formattedMessage() );
                return false;
        } catch( ParserException& pe ) {
                string msg( "Parser: fatal exception " );
                msg.append( pe.message() );
                traceUI->alert( msg );
                return false;
        } catch( TextureMapException e ) {
                string msg( "Texture mapping exception: " );
                msg.append( e.message() );
                traceUI->alert( msg );
                return false;
        }

        if (!sceneLoaded())
                return false;

        return true;
}

void RayTracer::traceSetup(int w, int h)
{
        size_t newBufferSize = w * h * 3;
        if (newBufferSize != buffer.size()) {
                bufferSize = newBufferSize;
                buffer.resize(bufferSize);
        }
        buffer_width = w;
        buffer_height = h;
        std::fill(buffer.begin(), buffer.end(), 0);
        m_bBufferReady = true;

        /*
         * Sync with TraceUI
         */

        threads = traceUI->getThreads();
        block_size = traceUI->getBlockSize();
        thresh = traceUI->getThreshold();
        samples = traceUI->getSuperSamples();
        aaThresh = traceUI->getAaThreshold();

        // YOUR CODE HERE
        // FIXME: Additional initializations

        //kdTree support
        kd_on = traceUI->kdSwitch();
        depth = traceUI->getMaxDepth();
        leaf_size = traceUI->getLeafSize();
        std::vector<Geometry *> objects;
        for ( std::vector<Geometry*>::const_iterator obj = objects.begin(); obj != objects.end(); obj++ ) {     
                objects.push_back(*obj);
        }
        //if the kdTree option enabled, build the tree
        if (kd_on) {
                scene->kdTree = new Node();
                scene->kdTree->buildTree(objects, scene->bounds(), depth, depth, leaf_size);
        }
}

/*
 * RayTracer::traceImage
 *
 *      Trace the image and store the pixel data in RayTracer::buffer.
 *
 *      Arguments:
 *              w:      width of the image buffer
 *              h:      height of the image buffer
 *
 */
void RayTracer::traceImage(int w, int h)
{
        // Always call traceSetup before rendering anything.
        traceSetup(w,h);
        // YOUR CODE HERE
        // FIXME: Start one or more threads for ray tracing
        //also implement anti-aliasing!
        //
                //shoot rays through the image plane
        // if (kd_on) {
        //         while (scene->kdTree != NULL) {
        //                 if (!scene->kdTree->findIntersection(r, 0, 1000)) {
        //                         return glm::dvec3 ret(0.0, 0.0, 0.0);
        //                 }  
        //         }

        // }


		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {


				glm::dvec3 S = tracePixel(i, j);
			}
		}

        // TIPS: Ideally, the traceImage should be executed asynchronously,
        //       i.e. returns IMMEDIATELY after working threads are launched.
        //
        //       An asynchronous traceImage lets the GUI update your results
        //       while rendering.
}

int RayTracer::aaImage()
{
        // YOUR CODE HERE
        // FIXME: Implement Anti-aliasing here
        //
        // TIP: samples and aaThresh have been synchronized with TraceUI by
        //      RayTracer::traceSetup() function

        //multiple samples per pixel by dividing the pixel into grid cells of equal area and tracing a ray out of the center of each grid cell
        //You can call traceRay multiple times in which ever function you want. aaImage is there for if you're doing adaptive super sampling and want to refine the image progressively


        // aaThresh;
        // glm::dvec3 colors[buffer_width][buffer_height];
        // for (int i = 0; i < buffer_width; i++) {
        //         for (int j = 0; j < buffer_height; j++) {
        //                 glm::dvec3 color = getPixel(i, j);
        //                 int diff = sqrt(aaThresh);
        //                 for (int k = 0; k < diff; k++) {
        //                         for (int h = 0; h < diff; h++) {
        //                                 if (k == 0 && h == 0) break;
        //                                 if (i + k >= buffer_width) break;
        //                                 if (j + h >= buffer_height) break;
        //                                 color += getPixel(i + k, j + h);   
        //                         }
                                
        //                 }
        //                 color /= aaThresh;
        //                 //setPixel(i, j, color);
        //                 colors[i][j] = color;
        //          }
        // }


        // for (int i = 0; i < buffer_width; i++) {
        //         for (int j = 0; j < buffer_height; j++) {
        //                 setPixel(i, j, colors[i][j]);
        //         }
        // }
        return 0;
}

bool RayTracer::checkRender()
{
        // YOUR CODE HERE
        // FIXME: Return true if tracing is done.
        //        This is a helper routine for GUI.
        //
        // TIPS: Introduce an array to track the status of each worker thread.
        //       This array is maintained by the worker threads.
        return true;
}

void RayTracer::waitRender()
{
        // YOUR CODE HERE
        // FIXME: Wait until the rendering process is done.
        //        This function is essential if you are using an asynchronous
        //        traceImage implementation.
        //
        // TIPS: Join all worker threads here.
}


glm::dvec3 RayTracer::getPixel(int i, int j)
{
        unsigned char *pixel = buffer.data() + ( i + j * buffer_width ) * 3;
        return glm::dvec3((double)pixel[0]/255.0, (double)pixel[1]/255.0, (double)pixel[2]/255.0);
}

void RayTracer::setPixel(int i, int j, glm::dvec3 color)
{
        unsigned char *pixel = buffer.data() + ( i + j * buffer_width ) * 3;

        pixel[0] = (int)( 255.0 * color[0]);
        pixel[1] = (int)( 255.0 * color[1]);
        pixel[2] = (int)( 255.0 * color[2]);
}

