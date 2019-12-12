#ifndef CaPTkInteractiveSegmentation_h
#define CaPTkInteractiveSegmentation_h

// The following header file is generated by CMake and thus it's located in
// the build directory. It provides an export macro for classes and functions
// that you want to be part of the public interface of your module.
#include <MitkCaPTkInteractiveSegmentationModuleExports.h>

#include "CaPTkInteractiveSegmentationAdapter.h"
#include "GeodesicTrainingSegmentation.h"

#include "mitkImage.h"
#include "mitkLabelSetImage.h"
#include "mitkDataStorage.h"

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>

/** \class CaPTkInteractiveSegmentation
 *  \brief Singleton class that runs the interactive segmentation 
 * algorithm and adds the result to the data storage
 */
class MITKCAPTKINTERACTIVESEGMENTATIONMODULE_EXPORT CaPTkInteractiveSegmentation /*final*/ : 
                                                    public QObject
{
    Q_OBJECT

public:
    CaPTkInteractiveSegmentation(
            mitk::DataStorage::Pointer dataStorage,
            QObject *parent = 0);

    ~CaPTkInteractiveSegmentation() {}

    /** \brief Runs the algorithm
     * 
     * Execute the algorithm in a background thread. When the
     * algorithm finishes, OnAlgorithmFinished() is called.
     * 
     * @param images a list of the co-registered input images
     * @param labels label image that contains the user drawn seeds
    */
    void Run(std::vector<mitk::Image::Pointer>& images, 
             mitk::LabelSetImage::Pointer& seeds);

    typedef struct Result 
    {
        // std::shared_ptr<GeodesicTrainingSegmentation::Coordinator<float,2>::Result> res2D;
        // std::shared_ptr<GeodesicTrainingSegmentation::Coordinator<float,3>::Result> res3D;
        mitk::LabelSetImage::Pointer seeds;
        mitk::LabelSetImage::Pointer segmentation;
        bool ok = true;
        std::string errorMessage = "";
    } Result;

public slots:
    /** \brief This function runs in the main thread when 
     * the algorithm is finished to add the result to the data storage
    */
    void OnAlgorithmFinished();

protected:

    /** \brief Runs the algorithm after the operations in Run
     * 
     * This can serve as a background thread. When the
     * algorithm finishes, OnAlgorithmFinished() is called.
     * 
     * @param images a list of the co-registered input images
     * @param labels label image that contains the user drawn seeds
     * @return a result struct that has either the 2D or 3D part populated
    */
    Result RunThread(std::vector<mitk::Image::Pointer>& images, 
                     mitk::LabelSetImage::Pointer& seeds);
    
    std::string FindNextAvailableSegmentationName();

    bool IsNumber(const std::string& s);

    bool m_IsRunning = false;
    QFutureWatcher<Result> m_Watcher;
    QFuture<Result> m_FutureResult;

    mitk::DataStorage::Pointer m_DataStorage;
};

#endif // ! CaPTkInteractiveSegmentation_h
