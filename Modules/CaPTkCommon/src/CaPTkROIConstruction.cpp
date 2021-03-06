#include <CaPTkROIConstruction.h>

#include <mitkImageCast.h>
#include <itkExtractImageFilter.h>
#include <mitkImageAccessByItk.h>
#include <mitkLabelSetImageConverter.h>
#include <mitkITKImageImport.h>

#include "CaPTkROIConstructionHelperBase.h"

captk::ROIConstruction::ROIConstruction()
{
    
}

captk::ROIConstruction::~ROIConstruction()
{
    
}

void captk::ROIConstruction::Update(
        mitk::LabelSetImage::Pointer input,
        float radius,
        float step)
{
    // ---- Save template image for GetNext() use
    m_MaskTemplate = mitk::Image::New();
    m_MaskTemplate->Initialize(
        mitk::ConvertLabelSetImageToImage(input.GetPointer())
    ); // This creates an empty copy, with the same meta-data

    m_MaskLabelSetCopy = input->GetActiveLabelSet()->Clone();

    // Create helper with the mask, by calling CreateHelper
    // AccessByItk calls the function with the ITK image (not MITK)
    // using the correct template
    AccessByItk(input, CreateHelper);

    // TODO: Pass labels and values to helper
    std::vector<std::string> labels;
    std::vector<int> values;
    this->GetLabelsAndValuesVectors(input, labels, values);
    m_Helper->SetValuesAndNames(values, labels);

    // Construct the ROI indices
    m_Helper->Update(
        radius,
        step);
}

bool captk::ROIConstruction::IsAtEnd()
{
    return m_Helper->IsAtEnd();
}

float captk::ROIConstruction::PopulateMask(mitk::LabelSetImage::Pointer& rMask)
{
    rMask->Initialize(m_MaskTemplate); // This creates an empty copy, with the same meta-data

    float weight = m_Helper->PopulateMask(rMask);

    // Change color of label
    auto label = rMask->GetActiveLabelSet()->GetActiveLabel();
    label->SetColor(m_MaskLabelSetCopy->GetLabel(label->GetValue())->GetColor());

    return weight;
}

void captk::ROIConstruction::GoToBegin()
{
    m_Helper->GoToBegin();
}

captk::ROIConstruction& captk::ROIConstruction::operator++() //prefix
{
    ++*m_Helper.get(); // actual operation, get() gets raw pointer, * gets the object, ++ is the operation
    return *this;
}

captk::ROIConstruction captk::ROIConstruction::operator++(int) //suffix(calls prefix)
{
    ROIConstruction tmp(*this);
    operator++(); // call prefix
    return tmp;
}

void captk::ROIConstruction::GetLabelsAndValuesVectors(
    mitk::LabelSetImage::Pointer mask,
    std::vector<std::string> &labels,
    std::vector<int> &values)
{
    // ---- Construct roi values and names from LabelSet
    mitk::LabelSet::Pointer labelSet = mask->GetActiveLabelSet();
    mitk::LabelSet::LabelContainerConstIteratorType it;
    for (it = labelSet->IteratorConstBegin();
         it != labelSet->IteratorConstEnd();
         ++it)
    {
        if (it->second->GetValue() != 0)
        {
            values.push_back(it->second->GetValue());
            labels.push_back(it->second->GetName());
        }
    }
}
