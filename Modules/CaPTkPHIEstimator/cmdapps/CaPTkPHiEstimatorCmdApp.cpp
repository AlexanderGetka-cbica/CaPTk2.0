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

#include <mitkCommandLineParser.h>
#include <mitkIOUtil.h>

#include <EGFRvIIISurrogateIndex.h>
#include <EGFRStatusPredictor.h>
#include <mitkImageToItk.h>

#include "itkCastImageFilter.h"

#include <algorithm>
#include <string>

/** \brief Example command-line app that demonstrates the example image filter
 *
 * This command-line app will take the first given image and add the
 * provided offset to each voxel.
 */

int main(int argc, char* argv[])
{
	mitkCommandLineParser parser;

	// Set general information about your command-line app
	parser.setCategory("CaPTk Cmd App Category");
	parser.setTitle("CaPTk PHI Estimator Cmd App");
	parser.setContributor("CBICA");
	parser.setDescription(
		"This command-line app takes the given perfusion image and a near_far mask and calculates the PHI Value.");

	// How should arguments be prefixed
	parser.setArgumentPrefix("--", "-");

	// Add arguments. Unless specified otherwise, each argument is optional.
	// See mitkCommandLineParser::addArgument() for more information.
	parser.addArgument(
		"input",
		"i",
		mitkCommandLineParser::String,
		"Input Perfusion image path",
		"Path to the input perfusion image",
		us::Any(),
		false); // false -> required parameter

	parser.addArgument(
		"mask",
		"m",
		mitkCommandLineParser::String,
		"Near Far Mask Image",
		"Path to mask image.",
		us::Any(),
		false);

	parser.addArgument( // optional
		"verbose",
		"v",
		mitkCommandLineParser::Bool,
		"Verbose Output",
		"Whether to produce verbose output");

	// Parse arguments. This method returns a mapping of long argument names to
	// their values.
	auto parsedArgs = parser.parseArguments(argc, argv);

	if (parsedArgs.empty())
		return EXIT_FAILURE; // Just exit, usage information was already printed.

	if (parsedArgs["input"].Empty() || parsedArgs["mask"].Empty())
	{
		MITK_INFO << parser.helpText();
		return EXIT_FAILURE;
	}

	// Parse, cast and set required arguments
	auto inFilename = us::any_cast<std::string>(parsedArgs["input"]);
	auto maskFilename = us::any_cast<std::string>(parsedArgs["mask"]);

	// Default values for optional arguments
	auto verbose = false;

	// Parse, cast and set optional arguments
	if (parsedArgs.end() != parsedArgs.find("verbose"))
		verbose = us::any_cast<bool>(parsedArgs["verbose"]);

	try
	{
		if (verbose)
			MITK_INFO << "Read input file";


		// read input perfusion image
		auto inImage = mitk::IOUtil::Load<mitk::Image>(inFilename);

		// exit if image could not be read
		if (inImage.IsNull())
		{
			MITK_ERROR << "Could not read \"" << inFilename << "\"!";
			return EXIT_FAILURE;
		}

		if (verbose)
			MITK_INFO << "Read mask file";

		// read near far mask
		auto maskImage = mitk::IOUtil::Load<mitk::Image>(maskFilename);

		// exit if mask could not be read
		if (maskImage.IsNull())
		{
			MITK_ERROR << "Could not read \"" << maskFilename << "\"!";
			return EXIT_FAILURE;
		}

		if (verbose)
			MITK_INFO << "Run PHI Estimation";

		// PHI Estimation starts from here

		// TODO: some cleanup could be done below
		// TODO: it should handle all image types
		VectorDouble EGFRStatusParams;
		EGFRStatusPredictor EGFRPredictor;
		using ImageType = itk::Image<float, 3>;
		typedef itk::Image<unsigned short, 3> MaskImageType;
		typedef itk::Image<float, 4> Float4DImage;
		typedef itk::Image<short, 4> PerfusionImageType;
		MaskImageType::Pointer maskimg;
		Float4DImage::Pointer perfImg;
		std::vector<ImageType::Pointer> Perfusion_Registered;
		std::vector<ImageType::IndexType> nearIndices, farIndices;

		// cast from mitk to itk image
		maskimg = mitk::ImageToItkImage<unsigned short, 3>(maskImage);
		PerfusionImageType::Pointer pimg = mitk::ImageToItkImage<short, 4>(inImage);

		// cast to float 4D image(algo needs this)
		typedef itk::CastImageFilter<PerfusionImageType, Float4DImage> CastFilterType;
		auto castFilter = CastFilterType::New();
		castFilter->SetInput(pimg);
		castFilter->Update();
		perfImg = castFilter->GetOutput();

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
		EGFRStatusParams = EGFRPredictor.PredictEGFRStatus<ImageTypeFloat3D, Float4DImage>
			(perfImg, Perfusion_Registered, nearIndices, farIndices, CAPTK::ImageExtension::NIfTI);

		// exit if results are empty
		if (EGFRStatusParams.empty())
		{
			MITK_ERROR << "PHI Estimation failed!";
			return EXIT_FAILURE;
		}

		if (verbose)
			MITK_INFO << "printing output";

		// print output
		std::cout << " PHI Value = " << EGFRStatusParams[0] << std::endl;
		std::cout << " Peak Height Ratio = " << EGFRStatusParams[1] / EGFRStatusParams[2] << std::endl;
		std::cout << " # Near Voxels = " << EGFRStatusParams[3] << std::endl;
		std::cout << " # far voxels = " << EGFRStatusParams[4] << std::endl;

		return EXIT_SUCCESS;
	}
	catch (const std::exception &e)
	{
		MITK_ERROR << e.what();
		return EXIT_FAILURE;
	}
	catch (...)
	{
		MITK_ERROR << "Unexpected error!";
		return EXIT_FAILURE;
	}
}