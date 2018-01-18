/**
 * @file nbc_test.cpp
 * @author Manish Kumar
 *
 * Test mlpackMain() of nbc_main.cpp.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#define BINDING_TYPE BINDING_TYPE_TEST

#include <mlpack/core.hpp>
static const std::string testName = "NBC";

#include <mlpack/core/util/mlpack_main.hpp>
#include <mlpack/methods/naive_bayes/nbc_main.cpp>
#include "test_helper.hpp"

#include <boost/test/unit_test.hpp>
#include "../test_tools.hpp"

using namespace mlpack;

struct NBCTestFixture
{
 public:
  NBCTestFixture()
  {
    // Cache in the options for this program.
    CLI::RestoreSettings(testName);
  }

  ~NBCTestFixture()
  {
    // Clear the settings.
    CLI::ClearSettings();
  }
};

BOOST_FIXTURE_TEST_SUITE(NBCMainTest, NBCTestFixture);

/**
 * Ensure that we get desired dimensions when both training
 * data and labels are passed.
 */
BOOST_AUTO_TEST_CASE(NBCOutputDimensionTest)
{
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  // Get the labels out.
  arma::Row<size_t> labels(inputData.n_cols);
  for (size_t i = 0; i < inputData.n_cols; ++i)
    labels[i] = inputData(inputData.n_rows - 1, i);

  // Delete the last row containing labels from input dataset.
  inputData.shed_row(inputData.n_rows - 1);

  arma::mat testData;
  if (!data::Load("testSet.csv", testData))
    BOOST_FAIL("Cannot load test dataset testSet.csv!");

  // Delete the last row containing labels from test dataset.
  testData.shed_row(testData.n_rows - 1);

  size_t testSize = testData.n_cols;

  // Input training data.
  SetInputParam("training", std::move(inputData));
  SetInputParam("labels", std::move(labels));

  // Input test data.
  SetInputParam("test", std::move(testData));

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_rows, 2);
}

/**
 * Check that last row of input file is used as labels
 * when labels are not passed specifically and results
 * are same from both label and labeless models.
 */
BOOST_AUTO_TEST_CASE(NBCLabelsLessDimensionTest)
{
  // Train NBC without providing labels.
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  // Get the labels out.
  arma::Row<size_t> labels(inputData.n_cols);
  for (size_t i = 0; i < inputData.n_cols; ++i)
    labels[i] = inputData(inputData.n_rows - 1, i);

  arma::mat testData;
  if (!data::Load("testSet.csv", testData))
    BOOST_FAIL("Cannot load test dataset testSet.csv!");

  // Delete the last row containing labels from test dataset.
  testData.shed_row(testData.n_rows - 1);

  size_t testSize = testData.n_cols;

  // Delete the last row containing labels from input dataset
  // and store it as a new dataset to be used while training
  // second model.
  arma::mat inputData2 = inputData;
  inputData2.shed_row(inputData2.n_rows - 1);

  // Create a copy of testData to be reused.
  arma::mat testData2 = testData;

  // Input training data.
  SetInputParam("training", std::move(inputData));

  // Input test data.
  SetInputParam("test", std::move(testData));

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_rows, 2);

  // Reset data passed.
  CLI::GetSingleton().Parameters()["training"].wasPassed = false;
  CLI::GetSingleton().Parameters()["test"].wasPassed = false;

  // Store outputs.
  arma::Row<size_t> output;
  arma::mat output_probs;
  output = std::move(CLI::GetParam<arma::Row<size_t>>("output"));
  output_probs = std::move(CLI::GetParam<arma::mat>("output_probs"));

  // Now train NBC with labels provided.

  // Input training data.
  SetInputParam("training", std::move(inputData2));
  SetInputParam("test", std::move(testData2));
  // Pass Labels.
  SetInputParam("labels", std::move(labels));

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_rows, 2);

  // Check that initial output and final output matrix
  // from two models are same.
  CheckMatrices(output, CLI::GetParam<arma::Row<size_t>>("output"));
  CheckMatrices(output_probs, CLI::GetParam<arma::mat>("output_probs"));
}

/**
 * Ensure that saved model can be used again.
 */
BOOST_AUTO_TEST_CASE(NBCModelReuseTest)
{
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  arma::mat testData;
  if (!data::Load("testSet.csv", testData))
    BOOST_FAIL("Cannot load test dataset testSet.csv!");

  // Delete the last row containing labels from test dataset.
  testData.shed_row(testData.n_rows - 1);

  size_t testSize = testData.n_cols;

  // Create a copy of testData to be reused.
  arma::mat testData2 = testData;

  // Input training data.
  SetInputParam("training", std::move(inputData));

  // Input test data.
  SetInputParam("test", std::move(testData));

  mlpackMain();

  arma::Row<size_t> output;
  arma::mat output_probs;
  output = std::move(CLI::GetParam<arma::Row<size_t>>("output"));
  output_probs = std::move(CLI::GetParam<arma::mat>("output_probs"));

  // Reset passed parameters.
  CLI::GetSingleton().Parameters()["training"].wasPassed = false;
  CLI::GetSingleton().Parameters()["test"].wasPassed = false;

  // Input trained model.
  SetInputParam("test", std::move(testData2));
  SetInputParam("input_model",
                std::move(CLI::GetParam<NBCModel>("output_model")));

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_rows, 2);

  // Check that initial output and final output
  // matrix using saved model are same.
  CheckMatrices(output, CLI::GetParam<arma::Row<size_t>>("output"));
  CheckMatrices(output_probs, CLI::GetParam<arma::mat>("output_probs"));
}

/**
 * Make sure only one of training data or pre-trained model is passed.
 */
BOOST_AUTO_TEST_CASE(NBCTrainingVerTest)
{
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  // Input training data.
  SetInputParam("training", std::move(inputData));

  mlpackMain();

  // Input pre-trained model.
  SetInputParam("input_model",
                std::move(CLI::GetParam<NBCModel>("output_model")));

  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

/**
 * Check that models trained with or without incremental
 * variance outputs same results
 */
BOOST_AUTO_TEST_CASE(NBCIncrementalVarianceTest)
{
  // Train NBC with incremental variance.
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  arma::mat testData;
  if (!data::Load("testSet.csv", testData))
    BOOST_FAIL("Cannot load test dataset testSet.csv!");

  // Delete the last row containing labels from test dataset.
  testData.shed_row(testData.n_rows - 1);

  size_t testSize = testData.n_cols;

  // Create a copy of inputData to be reused.
  arma::mat inputData2 = inputData;

  // Create a copy of testData to be reused.
  arma::mat testData2 = testData;

  // Input training data.
  SetInputParam("training", std::move(inputData));

  // Input test data.
  SetInputParam("test", std::move(testData));
  SetInputParam("incremental_variance", (bool) true);

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_rows, 2);

  // Reset data passed.
  CLI::GetSingleton().Parameters()["training"].wasPassed = false;
  CLI::GetSingleton().Parameters()["incremental_variance"].wasPassed = false;
  CLI::GetSingleton().Parameters()["test"].wasPassed = false;

  // Store outputs.
  arma::Row<size_t> output;
  arma::mat output_probs;
  output = std::move(CLI::GetParam<arma::Row<size_t>>("output"));
  output_probs = std::move(CLI::GetParam<arma::mat>("output_probs"));

  // Now train NBC without incremental_variance.

  // Input training data.
  SetInputParam("training", std::move(inputData2));
  SetInputParam("test", std::move(testData2));
  SetInputParam("incremental_variance", (bool) false);

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output_probs").n_rows, 2);

  // Check that initial output and final output matrix
  // from two models are same.
  CheckMatrices(output, CLI::GetParam<arma::Row<size_t>>("output"));
  CheckMatrices(output_probs, CLI::GetParam<arma::mat>("output_probs"));
}

BOOST_AUTO_TEST_SUITE_END();
