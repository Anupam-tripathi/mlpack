/**
 * @file param_checks.hpp
 * @author Ryan Curtin
 *
 * A set of utility functions to check parameter values for mlpack programs.
 * These are meant to be used as the first part of an mlpackMain() function, to
 * validate parameters.
 */
#ifndef MLPACK_CORE_UTIL_PARAM_CHECKS_HPP
#define MLPACK_CORE_UTIL_PARAM_CHECKS_HPP

#include <mlpack/prereqs.hpp>

namespace mlpack {
namespace util {

/**
 * Require that only one of the given parameters in the constraints set was
 * passed to the CLI object; otherwise, issue a warning or fatal error,
 * optionally with the given custom error message.
 *
 * This uses the correct binding type name for each parameter (i.e.
 * '--parameter' for CLI bindings, 'parameter' for Python bindings).
 *
 * If you use a custom error message, be aware that the given output will be
 * similar to, for example:
 *
 * @code
 * Must specify one of '--reference_file (-r)' or '--input_model_file (-m)';
 * <custom error message here>!
 * @endcode
 *
 * so when you write your custom error message, be sure that the sentence makes
 * sense.  The custom error message should not have a capitalized first
 * character and no ending punctuation (a '!' will be added by this function).
 *
 * @param constraints Set of parameters from which only one should be passed.
 * @param fatal If true, output goes to Log::Fatal instead of Log::Warn and an
 *     exception is thrown.
 * @param customErrorMessage Error message to append.
 */
void RequireOnlyOnePassed(
    const std::vector<std::string>& constraints,
    const bool fatal = true,
    const std::string& customErrorMessage = "");

/**
 * Require that at least one of the given parameters in the constraints set was
 * passed to the CLI object; otherwise, issue a warning or fatal error,
 * optionally with the given custom error message.
 *
 * This uses the correct binding type name for each parameter (i.e.
 * '--parameter' for CLI bindings, 'parameter' for Python bindings).
 *
 * This can be used with a set of only one constraint and the output is still
 * sensible.
 *
 * If you use a custom error message, be aware that the given output will be
 * similar to, for example:
 *
 * @code
 * Should pass one of '--codes_file (-c)', '--dictionary_file (-d)', or
 * '--output_model_file (-M)'; <custom error message>!
 * @endcode
 *
 * so when you write your custom error message, be sure that the sentence makes
 * sense.  The custom error message should not have a capitalized first
 * character and no ending punctuation (a '!' will be added by this function).
 *
 * @param constraints Set of parameters from which only one should be passed.
 * @param fatal If true, output goes to Log::Fatal instead of Log::Warn and an
 *     exception is thrown.
 * @param customErrorMessage Error message to append.
 */
void RequireAtLeastOnePassed(
    const std::vector<std::string>& constraints,
    const bool fatal = true,
    const std::string& customErrorMessage = "");

/**
 * Require that a given parameter is in a set of allowable parameters.  This is
 * probably most useful with T = std::string.  If fatal is true, then an
 * exception is thrown.  An error message is not optional and must be specified.
 * The error message does _not_ need to specify the values in the set; this
 * function will already output them.  So, for example, the output may be
 * similar to:
 *
 * @code
 * Invalid value of '--weak_learner (-w)' specified ('something'); <error
 * message>; must be one of 'decision_stump', or 'perceptron'!
 * @endcode
 *
 * so when you write the error message, make sure that the message makes sense.
 * For example, in the message above, a good error message might be "unknown
 * weak learner type".
 *
 * @tparam T Type of parameter.
 * @param paramName Name of parameter to check.
 * @param set Set of valid values for parameter.
 * @param fatal If true, an exception is thrown and output goes to Log::Fatal.
 * @param errorMessage Error message to output.
 */
template<typename T>
void RequireParamInSet(const std::string& paramName,
                       const std::vector<T>& set,
                       const bool fatal,
                       const std::string& errorMessage);

/**
 * Require that a given parameter satisfies the given conditional function.
 * This is useful for, e.g., checking that a given parameter is greater than 0.
 * If fatal is true, then an exception is thrown.  An error message is not
 * optional and must be specified.  The error message should specify, in clear
 * terms, what the value of the parameter *should* be.  So, for example, the
 * output may be similar to:
 *
 * @code
 * Invalid value of '--iterations (-i)' specified (-1); <error message>!
 * @endcode
 *
 * and in this case a good error message might be "number of iterations must be
 * positive".  Be sure that when you write the error message, the message makes
 * sense.
 *
 * @tparam T Type of parameter to check.
 * @param paramName Name of parameter to check.
 * @param conditional Function to use to check parameter value; should return
 *      'true' if the parameter value is okay.
 * @param fatal If true, an exception is thrown and output goes to Log::Fatal.
 * @param errorMessage Error message to output.
 */
template<typename T>
void RequireParamValue(const std::string& paramName,
                       const std::function<bool(T)>& conditional,
                       const bool fatal,
                       const std::string& errorMessage);

/**
 * Report that a parameter is ignored, if each of the constraints given are
 * satisfied.  The constraints should be a set of string/bool pairs.  If all of
 * the constraints are true, and the given parameter in 'paramName' is passed,
 * then a warning will be issued noting that the parameter is ignored.  The
 * warning will go to Log::Warn.
 *
 * @param constraints Set of constraints.
 * @param paramName Name of parameter to check.
 */
void ReportIgnoredParam(
    const std::vector<std::pair<std::string, bool>>& constraints,
    const std::string& paramName);

} // namespace util
} // namespace mlpack

// Include implementation.
#include "param_checks_impl.hpp"

#endif
