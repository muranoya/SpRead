#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <FL/Fl_Widget.H>
#include <utility>
#include <functional>
#include <memory>
#include "SpRead.hpp"
#include "BasicImage.hpp"
#include "Point.hpp"
#include "Uncopyable.hpp"

class Viewer : public Fl_Widget, private Uncopyable
{
public:
    typedef std::function<void(void)>
        NextImageReqCB;
    typedef std::function<void(void)>
        PrevImageReqCB;
    typedef std::function<void(int)>
        ChangeNumOfImagesCB;
    typedef std::function<void(const StringVec&)>
        OpenImageFilesCB;
    typedef std::function<void(void)>
        ChangeViewerStatusCB;

    enum FeedPageMode
    {
        MouseClickPosition,
        MouseButton,
    };
    enum ViewMode
    {
        ActualSize,
        FittingWindow,
        FittingWidth,
        CustomScale,
    };
    enum ScalingMode
    {
        NearestNeighbor,
        Bilinear,
        Bicubic,
    };

    explicit Viewer(int x, int y, int w, int h);
    ~Viewer();
    
    void showImages(const std::shared_ptr<BasicImage> &img_l,
            const std::shared_ptr<BasicImage> &img_r);

    void setScalingMode(ScalingMode mode);
    ScalingMode getScalingMode() const;

    void setViewMode(ViewMode mode, double factor);
    void setViewMode(ViewMode mode);
    double getCustomScaleFactor() const;
    ViewMode getViewMode() const;

    void setSpreadView(bool spread);
    bool getSpreadView() const;

    void setRightbindingView(bool rbind);
    bool getRightbindingView() const;

    void setAutoAdjustSpread(bool aas);
    bool getAutoAdjustSpread() const;

    void setFeedPageMode(FeedPageMode mode);
    FeedPageMode getFeedPageMode() const;

    void setMonochromeMode(bool mode);
    bool getMonochromeMode() const;

    void setNextImageCB(NextImageReqCB cd);
    void setPrevImageCB(PrevImageReqCB cd);
    void setChangeNumOfImagesCB(ChangeNumOfImagesCB cb);
    void setOpenImageFilesCB(OpenImageFilesCB cb);
    void setChangeViewerStatusCB(ChangeViewerStatusCB cb);

    /* override */
    void draw();
    void resize(int x, int y, int w, int h);
    int handle(int event);

private:
    NextImageReqCB nextImgReq;
    PrevImageReqCB prevImgReq;
    ChangeNumOfImagesCB changeNumOfImages;
    OpenImageFilesCB openImageFiles;
    ChangeViewerStatusCB changeViewerStatus;

    std::shared_ptr<BasicImage> based_imgs[2];
    BasicImage *scaled_imgs[2];
    int img_num;
    ScalingMode scale_mode;
    ViewMode view_mode;
    double scale_factor;
    bool spread_view;
    bool rbind_view;
    bool autospread;
    FeedPageMode fp_mode;
    bool monomode;
    bool is_drag_img;
    Point click_pos;
    Point click2_pos;
    Point move_pos;
    Point img_pos;
    const double drag_detect_time;

    static void drag_check(void *arg);

    int mousePressEvent();
    int mouseMoveEvent();
    int mouseReleaseEvent();
    int keyPressEvent();

    void splitWithNewLine(const std::string &str,
            StringVec &l);

    bool calc_size(const std::shared_ptr<BasicImage> &img1,
            const std::shared_ptr<BasicImage> &img2,
        int *cimg_w, int *cimg_h, int *img_num, double *scale) const;
    void rescaling();
    void drawImageToWidget(const Point &pos, const BasicImage &img);
};

#endif // VIEWER_HPP
