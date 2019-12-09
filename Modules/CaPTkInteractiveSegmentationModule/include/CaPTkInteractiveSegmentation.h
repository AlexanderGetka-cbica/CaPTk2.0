#ifndef CaPTkInteractiveSegmentation_h
#define CaPTkInteractiveSegmentation_h

// The following header file is generated by CMake and thus it's located in
// the build directory. It provides an export macro for classes and functions
// that you want to be part of the public interface of your module.
#include <MitkCaPTkInteractiveSegmentationModuleExports.h>

#include "mitkImage.h"
#include "mitkLabelSetImage.h"
#include "mitkDataStorage.h"

#include <QObject>

/** \class CaPTkInteractiveSegmentation
 *  \brief Singleton class that runs the interactive segmentation 
 * algorithm and adds the result to the data storage
 */
class /*MITKCAPTKINTERACTIVESEGMENTATIONMODULE_EXPORT*/ CaPTkInteractiveSegmentation final : 
                                                    public QObject
{
    Q_OBJECT

public:
    /** \brief Get the instance of the singleton class 
    */
    static CaPTkInteractiveSegmentation & getInstance() {
        static CaPTkInteractiveSegmentation * _instance = 0;
        if ( _instance == 0 ) {
            _instance = new CaPTkInteractiveSegmentation();
        }
        return *_instance;
    }

    virtual ~CaPTkInteractiveSegmentation() {}

    /** \brief Runs the algorithm
     * 
     * Execute the algorithm in a background thread. When the
     * algorithm finishes, OnAlgorithmFinished() is called.
     * 
     * @param images a list of the co-registered input images
     * @param labels label image that contains the user drawn seeds
    */
    void Run(std::vector<mitk::Image::Pointer> images, 
             mitk::LabelSetImage::Pointer labels);

protected slots:
    /** \brief This function runs in the main thread when 
     * the algorithm is finished to add the result to the data storage
    */
    void OnAlgorithmFinished();

private:
    /* These are needed to make the class singleton */
    CaPTkInteractiveSegmentation(/*QObject *parent = 0*/) {}
    // CaPTkInteractiveSegmentation(const CaPTkInteractiveSegmentation&) {}
    // CaPTkInteractiveSegmentation& (const CaPTkInteractiveSegmentation&) {
    //     return *this;
    // }
};

#endif // ! CaPTkInteractiveSegmentation_h
