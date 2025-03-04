#pragma once

    #include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <vector>
public static final float mapRange(float inMin, float inMax, float outMin, float outMax, float a) {
        
        
            float slope = 1.0f * (outMax - outMin) / (inMax - inMin);
            return outMin + slope * (a - inMin);
    }
    
    /**
     * Checks if point is inside a box
     * @param point Point to check
     * @param boxOrigin Box's top left corner
     * @param boxSize Box's size
     * @return 
     */
    public static final boolean pointInBox(V2f point, V2f boxOrigin, V2f boxSize) {
        
        return (
        
                point.X > boxOrigin.X && point.X < boxOrigin.X + boxSize.X &&
                point.Y > boxOrigin.Y && point.Y < boxOrigin.Y + boxSize.Y
        );
    }
 
    /**
     * Checks if a point is inside a triangle
     * @param point Point to check
     * @param a Point A of the triangle
     * @param b Point B of the triangle
     * @param c Point C of the triangle
     * @return 
     */
    public static final boolean pointInTriangle(V2f point, V2f a, V2f b, V2f c) {

        return !(
                V2f.cross(V2f.sub(b, a), V2f.sub(point, a)) > 0 || 
                V2f.cross(V2f.sub(c, b), V2f.sub(point, b)) > 0 || 
                V2f.cross(V2f.sub(a, c), V2f.sub(point, c)) > 0
        );
    }
    
    /**
     * Sorts 2D vertices based on their angle relative to the origin
     * @param origin Origin
     * @param verts List of verts to be sorted
     * @param cw Sort clockwise?
     * @return 
     */
    public static final V2f[] sort2D(V2f origin, List<V2f> verts, boolean cw) {
    
       Collections.sort(verts, (a, b) -> {
           
            double a1 = (Math.toDegrees(Math.atan2(a.X - origin.X, a.Y - origin.Y)) + 360) % 360;
            double a2 = (Math.toDegrees(Math.atan2(b.X - origin.X, b.Y - origin.Y)) + 360) % 360;
            return (int)(cw ? (a1 - a2) : (a2 - a1));
       });
        
       V2f[] out = new V2f[verts.size()];
       for (int i = 0; i < verts.size(); i++)
           out[i] = verts.get(i);
       
        return out;
    }
    

inline void frontface_towards_origin(bool is_cw, glm::vec3 origin, std::vector<glm::vec3>& vertices, std::vector<int>& indices) {

    for (int i = 0; i < indices.size(); i += 3) {
        
        glm::vec3 normal;
        
        /* Get the normal depending on winding order */
        if (is_cw) normal = glm::cross(
            vertices[indices[i + 2]] - vertices[indices[i]],
            vertices[indices[i + 1]] - vertices[indices[i]]
        );
        
        else normal = glm::cross(
            vertices[indices[i + 1]] - vertices[indices[i]],
            vertices[indices[i + 2]] - vertices[indices[i]]
        );
        
        /* Flip if not front facing */
        if (glm::dot((vertices[indices[i]] - origin), normal) <= 0) {
            
            int tmp = indices[i + 2];
            indices[i + 2] = indices[i + 1];
            indices[i + 1] = tmp;
        }
    }
}
    
    /**
     * Calculates texture coords based on input vertices
     * @param verts Array of vertices
     * @param textureZoom Factor of zoom of the texture
     * @return Texture coords
     */
    public static final V2f[] toTexCoords(V2f[] verts, float textureZoom) {
        
        V2f[] texCoords = new V2f[verts.length];
        
        /* Find the smallest bounding box */
        V2f pMin = new V2f(Float.MAX_VALUE, Float.MAX_VALUE);
        V2f pMax = new V2f(Float.MIN_VALUE, Float.MIN_VALUE);
        
        for (V2f v : verts) {
            pMin.X = minf(v.X, pMin.X);
            pMax.X = maxf(v.X, pMax.X);    
            
            pMin.Y = minf(v.Y, pMin.Y);
            pMax.Y = maxf(v.Y, pMax.Y);
        }
        
        float side = maxf(V2f.sub(pMin, pMax).X, V2f.sub(pMin, pMax).Y);
        V2f minBound = pMin;
        V2f maxBound = new V2f(minBound.X + side, minBound.Y + side);
        
        for (int i = 0; i < verts.length; i++) 
            /* Map to range: minBound -> 0; minBound+size -> side / textureZoom */
            texCoords[i] = new V2f(
                    mapRange(minBound.X, maxBound.X, 0,  side / textureZoom, verts[i].X),
                    mapRange(minBound.Y, maxBound.Y, side / textureZoom, 0, verts[i].Y) /* flip texture on Y axis */
            );
        
        return texCoords;
    }
    
    /**
     * Triangulates a polygon described by verts
     * @param verts array of vertices
     * @return 
     */
    public static final int[] triangulatePolygon2D(V2f[] verts) {
        
        if (verts.length < 3) 
            return null; /* Throw */
        
        /* Prepare list */
        LinkedList<Integer> activeVerts = new LinkedList<>();
        for (int i = 0; i < verts.length; i++) activeVerts.add(i);
        
        int[] outIdx = new int[(verts.length - 2) * 3];
        int idxPtr = 0;
      
        
        /* Ear clipping algorithm */
        while (activeVerts.size() > 3) {
            
            for (int i = 0; i < activeVerts.size(); i++) {
                
                int prevIdx = (i - 1 + activeVerts.size()) % activeVerts.size();
                int nextIdx = (i + 1 + activeVerts.size()) % activeVerts.size();
                
                V2f prevVert = verts[activeVerts.get(prevIdx)];
                V2f currVert = verts[activeVerts.get(i)];
                V2f nextVert = verts[activeVerts.get(nextIdx)];
                
                /* Angle check */
                if (V2f.cross(V2f.sub(prevVert, currVert), V2f.sub(nextVert, currVert)) < 0)
                    continue;
                
                /* Point inside triangle */
                boolean isValidEar = true;
                for (int v = 0; v < verts.length; v++) {
                    
                    if (
                        v == activeVerts.get(prevIdx) ||
                        v == activeVerts.get(i)       ||
                        v == activeVerts.get(nextIdx)
                    ) continue;
                    
                    if (pointInTriangle(verts[v], prevVert, currVert, nextVert)) {
                        
                        /* Not clipable */
                        isValidEar = false;
                        break;
                    }
                }
                
                if (!isValidEar)
                    continue;
                
                /* Clip */
                outIdx[idxPtr++] = activeVerts.get(prevIdx);
                outIdx[idxPtr++] = activeVerts.get(i);
                outIdx[idxPtr++] = activeVerts.get(nextIdx);
                activeVerts.remove(i);
                break;   
            }
        }
       
        /* Last triangle left */
        outIdx[idxPtr++] = activeVerts.get(0);
        outIdx[idxPtr++] = activeVerts.get(1);
        outIdx[idxPtr++] = activeVerts.get(2);
        
        
        return outIdx;
    }
    
    
