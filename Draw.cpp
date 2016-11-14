#include "Draw.h"


void calc_mvp(Matrix& mvp)
{
    /* Define projection properties. */
    float degreesToRadiansCoefficient = atanf(1) / 45;                            /* Coefficient to recalculate degrees to radians.      */
    float frustum_fovy                = 45.0f;                                    /* 45 degrees field of view in the y direction.        */
    float frustum_aspect              = (float)window_width/(float)window_height; /* Aspect ratio.                                       */
    float frustum_z_near              = 0.01f;                                    /* How close the viewer is to the near clipping plane. */
    float frustum_z_far               = 100.0f;                                   /* How far the viewer is from the far clipping plane.  */
    float camera_distance             = 2.5f;                                     /* Distance from camera to scene center.               */

    /* Matrix that stores temporary matrix data for translation transformations. */
    Matrix mat4_translate  = Matrix::createTranslation(-0.5, -0.5, -0.5);

    /* Matrix that stores temporary matrix data for scale transformations. */
    Matrix mat4_scale      = Matrix::createScaling    ( 2.0,  2.0,  2.0);

    /* Matrix that transforms the vertices from model space to world space. */
    /* Translate and scale coordinates from [0..1] to [-1..1] range for full visibility. */
    Matrix mat4_model_view = mat4_scale * mat4_translate;

    /* Pull the camera back from the scene center. */
    mat4_model_view[14] -= float(camera_distance);

    /* Create the perspective matrix from frustum parameters. */
    Matrix mat4_perspective = Matrix::matrixPerspective(degreesToRadiansCoefficient * frustum_fovy, frustum_aspect, frustum_z_near, frustum_z_far);

    /* MVP (Model View Perspective) matrix is a result of multiplication of Perspective Matrix by Model View Matrix. */
    mvp = mat4_perspective * mat4_model_view;
}


void setupGraphics(int width, int height)
{
    /* Store window width and height. */
    window_width  = width;
    window_height = height;

    /* Specify one byte alignment for pixels rows in memory for pack and unpack buffers. */
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glPixelStorei(GL_PACK_ALIGNMENT,   1));

    GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
    /* Initialize model view projection matrix. */
    calc_mvp(mvp);

    SetUpSphereUpdater();
    SetUpScalarField();
    // SetUpMarchingCubesCells();
    // SetUpMarchingCubesTriangles();
    SetUpMarchingCubes();

    /* Enable facet culling, depth testing and specify front face for polygons. */
    GL_CHECK(glEnable   (GL_DEPTH_TEST));
    GL_CHECK(glEnable   (GL_CULL_FACE ));
    GL_CHECK(glFrontFace(GL_CW        ));

    /* Start counting time. */
    timer.reset();
}

/** Draws one frame. */
void renderFrame(void)
{
    /* Update time. */
    model_time = timer.getTime();

    /* Clear the buffers that we are going to render to in a moment. */
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    DrawSphereUpdater();
    DrawScalarField();
    // DrawMarchingCubesCells();
    // DrawMarchingCubesTriangles();
    DrawMarchingCubes();

}

/** Deinitialises OpenGL ES environment. */
void cleanup()
{

    CleanUpSphereUpdater();
    CleanUpScalarField();
    CleanUpMarchingCubesCells();
    CleanUpMarchingCubesTriangles();
    CleanUpMarchingCubes();
}



