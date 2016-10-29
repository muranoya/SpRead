#ifndef IMAGE_H
#define IMAGE_H

#include "BasicImage.hpp"

/* Nearest Neighbor */
BasicImage *nn(const BasicImage &src, double s);
/* Bilinear */
BasicImage *bl(const BasicImage &src, double s);
/* Bicubic */
BasicImage *bc(const BasicImage &src, double s);

#endif // IMAGE_H
