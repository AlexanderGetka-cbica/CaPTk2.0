/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef PhiEstimator_h
#define PhiEstimator_h

// The following header file is generated by CMake and thus it's located in
// the build directory. It provides an export macro for classes and functions
// that you want to be part of the public interface of your module.

#include <MitkCaPTkPHIEstimatorExports.h>

#include <mitkImage.h>
#include <mitkImageCast.h>
#include <mitkLogMacros.h>
#include <mitkLabelSetImageConverter.h>

#include "itkImage.h"


namespace captk
{
/** \brief Runs PHI Estimation algorithm 
 * \param imageItk the input ITK image
 * \param mask mitk mask image
 * \param EGFRStatusParams, std::vector of results as double
 */
class MITKCAPTKPHIESTIMATOR_EXPORT PhiEstimator
{
public:
	template <class TPixel, unsigned int VDim>
	static void Run(itk::Image<TPixel, VDim>* imageItk, 
		mitk::LabelSetImage::Pointer& mask, 
		std::vector<typename itk::Image<float, 3>::IndexType> &nearIndices,
		std::vector<typename itk::Image<float, 3>::IndexType> &farIndices,
	std::vector<double>& EGFRStatusParams)
	{
		using ImageType = itk::Image<TPixel, VDim>;
		using MaskImageType = itk::Image<float, 3>;

		// PHI Estimation starts from here

		EGFRStatusPredictor EGFRPredictor;

		std::vector<MaskImageType::Pointer> Perfusion_Registered; // don't know where this is used in the algo, although the algo needs it
		
		// Check if the seg has only label 1 & 2
		bool found1 = false;
		bool found2 = false;
		int foundTotal = 0;
		auto labelSet = mask->GetActiveLabelSet();
		mitk::LabelSet::LabelContainerConstIteratorType it;
		for (it = labelSet->IteratorConstBegin();
			it != labelSet->IteratorConstEnd();
			++it)
		{
			if (it->second->GetValue() == 1) { found1 = true; }
			if (it->second->GetValue() == 2) { found2 = true; }
			if (it->second->GetValue() != 0) { foundTotal++; }
		}
		if (!found1)
		{
			throw std::exception("Missing label 1");
		}
		if (!found2)
		{
			throw std::exception("Missing label 2");
		}
		if (foundTotal != 2)
		{
			throw std::exception("The mask should only have 2 labels");
		}

		// Make mask 3D itk
		MaskImageType::Pointer maskimg;
		mitk::CastToItkImage<MaskImageType>(
			mitk::ConvertLabelSetImageToImage(mask.GetPointer()).GetPointer(),
			maskimg
		);

		//store near and far indices in vector to be passed to algo
		itk::ImageRegionIteratorWithIndex< MaskImageType > maskIt(maskimg, maskimg->GetLargestPossibleRegion());
		for (maskIt.GoToBegin(); !maskIt.IsAtEnd(); ++maskIt)
		{
			if (maskIt.Get() == 1)
			{
				nearIndices.push_back(maskIt.GetIndex());
			}
			else if (maskIt.Get() == 2)
				farIndices.push_back(maskIt.GetIndex());
		}

		// pass the arguments to algo, this does the actual calculation
		EGFRStatusParams = EGFRPredictor.PredictEGFRStatus<MaskImageType, ImageType>
			(imageItk, Perfusion_Registered, nearIndices, farIndices, CAPTK::ImageExtension::NIfTI);

		// exit if results are empty
		if (EGFRStatusParams.empty())
		{
			MITK_ERROR << "PHI Estimation failed!";
			throw("PHI Estimation failed!");
		}
	}

};//class

}//namespace
#endif//PhiEstimator_h