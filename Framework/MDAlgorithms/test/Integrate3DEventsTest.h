// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_MDEVENTS_INTEGRATE_3D_EVENTS_TEST_H_
#define MANTID_MDEVENTS_INTEGRATE_3D_EVENTS_TEST_H_

#include "MantidDataObjects/PeakShapeEllipsoid.h"
#include "MantidKernel/V3D.h"
#include "MantidMDAlgorithms/Integrate3DEvents.h"

#include <cxxtest/TestSuite.h>
#include <random>

using namespace Mantid;
using namespace Mantid::DataObjects;
using namespace Mantid::Kernel;
using namespace Mantid::MDAlgorithms;

class Integrate3DEventsTest : public CxxTest::TestSuite {
public:
  // Test support class for integration of events using ellipsoids aligned
  // with the principal axes of the events near a peak.  This test
  // generates some poorly distributed synthetic data, and checks that
  // expected integration results are obtained using either fixed size
  // ellipsoids, or using ellipsoids with axis half-lengths set to
  // three standard deviations.
  void test_integrateMainPeaksWithFixedRadiiandDefaultScaledRadii() {
    double inti_all[] = {755, 704, 603};
    double sigi_all[] = {27.4773, 26.533, 24.5561};

    double inti_some[] = {692, 649, 603};
    double sigi_some[] = {27.4590, 26.5141, 24.5561};

    // synthesize three peaks

    V3D peak_1(10, 0, 0);
    V3D peak_2(0, 5, 0);
    V3D peak_3(0, 0, 4);
    std::vector<std::pair<std::pair<double, double>, V3D>> peak_q_list{
        {std::make_pair(1., 1.), peak_1},
        {std::make_pair(1., 1.), peak_2},
        {std::make_pair(1., 1.), peak_3}};

    // synthesize a UB-inverse to map
    DblMatrix UBinv(3, 3, false); // Q to h,k,l
    UBinv.setRow(0, V3D(.1, 0, 0));
    UBinv.setRow(1, V3D(0, .2, 0));
    UBinv.setRow(2, V3D(0, 0, .25));

    // synthesize events around the
    // peaks.  All events with in one
    // unit of the peak.  755 events
    // around peak 1, 704 events around
    // peak 2, and 603 events around
    // peak 3.
    std::vector<std::pair<std::pair<double, double>, V3D>> event_Qs;
    for (int i = -100; i <= 100; i++) {
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_1 + V3D((double)i / 100.0, 0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_2 + V3D((double)i / 100.0, 0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_3 + V3D((double)i / 100.0, 0, 0))));

      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_1 + V3D(0, (double)i / 200.0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_2 + V3D(0, (double)i / 200.0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_3 + V3D(0, (double)i / 200.0, 0))));

      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_1 + V3D(0, 0, (double)i / 300.0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_2 + V3D(0, 0, (double)i / 300.0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_3 + V3D(0, 0, (double)i / 300.0))));
    }

    for (int i = -50; i <= 50; i++) {
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_1 + V3D(0, (double)i / 147.0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_2 + V3D(0, (double)i / 147.0, 0))));
    }

    for (int i = -25; i <= 25; i++) {
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(2., 1.), V3D(peak_1 + V3D(0, 0, (double)i / 61.0))));
    }

    double radius = 1.3;
    Integrate3DEvents integrator(peak_q_list, UBinv, radius);

    integrator.addEvents(event_Qs, false);

    // With fixed size ellipsoids, all the
    // events are counted.
    bool specify_size = true;
    double peak_radius = 1.2;
    double back_inner_radius = 1.2;
    double back_outer_radius = 1.3;
    std::vector<double> new_sigma;
    std::vector<Kernel::V3D> E1Vec;
    double inti;
    double sigi;
    for (size_t i = 0; i < peak_q_list.size(); i++) {
      auto shape = integrator.ellipseIntegrateEvents(
          E1Vec, peak_q_list[i].second, specify_size, peak_radius,
          back_inner_radius, back_outer_radius, new_sigma, inti, sigi);
      TS_ASSERT_DELTA(inti, 2 * inti_all[i], 0.1);
      TS_ASSERT_DELTA(sigi, sigi_all[i], 0.01);

      auto ellipsoid_shape = boost::dynamic_pointer_cast<
          const Mantid::DataObjects::PeakShapeEllipsoid>(shape);
      TSM_ASSERT("Expect to get back an ellipsoid shape", ellipsoid_shape);
    }

    // The test data is not normally distributed,
    // so with 3 sigma half-axis sizes, we miss
    // some counts
    specify_size = false;
    for (size_t i = 0; i < peak_q_list.size(); i++) {
      integrator.ellipseIntegrateEvents(
          E1Vec, peak_q_list[i].second, specify_size, peak_radius,
          back_inner_radius, back_outer_radius, new_sigma, inti, sigi);
      TS_ASSERT_DELTA(inti, 2 * inti_some[i], 0.1);
      TS_ASSERT_DELTA(sigi, sigi_some[i], 0.01);
    }
  }

  void test_satellites() {
    double inti_all[] = {161, 368.28, 273.28};
    double sigi_all[] = {12.6885, 21.558, 19.2287};

    double inti_some[] = {150, 241, 186};
    double sigi_some[] = {12.4900, 20.0749, 17.5499};

    // synthesize three peaks

    V3D peak_1(6, 0, 0);
    V3D peak_2(0, 5, 0);
    V3D peak_3(0, 0, 4);
    std::vector<std::pair<std::pair<double, double>, V3D>> peak_q_list{
        {std::make_pair(1., 1.), peak_1},
        {std::make_pair(1., 1.), peak_2},
        {std::make_pair(1., 1.), peak_3}};

    // synthesize a UB-inverse to map
    DblMatrix UBinv(3, 3, false); // Q to h,k,l
    UBinv.setRow(0, V3D(.1, 0, 0));
    UBinv.setRow(1, V3D(0, .2, 0));
    UBinv.setRow(2, V3D(0, 0, .25));

    std::vector<V3D> hkl_list{UBinv * peak_1, UBinv * peak_2, UBinv * peak_3};

    // synthesize a ModHKL
    DblMatrix ModHKL(3, 3, false); // Q to h,k,l
    ModHKL.setRow(0, V3D(0.4, 0, 0));
    ModHKL.setRow(1, V3D(0, 0, 0));
    ModHKL.setRow(2, V3D(0, 0, 0));

    std::vector<V3D> mnp_list{V3D(-1, 0, 0), V3D(0, 0, 0), V3D(0, 0, 0)};

    // synthesize events around the
    // peaks.  All events with in one
    // unit of the peak.  755 events
    // around peak 1, 704 events around
    // peak 2, and 603 events around
    // peak 3.
    std::vector<std::pair<std::pair<double, double>, V3D>> event_Qs;
    for (int i = -100; i <= 100; i++) {
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_1 + V3D((double)i / 100.0, 0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_2 + V3D((double)i / 100.0, 0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_3 + V3D((double)i / 100.0, 0, 0))));

      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_1 + V3D(0, (double)i / 200.0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_2 + V3D(0, (double)i / 200.0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_3 + V3D(0, (double)i / 200.0, 0))));

      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_1 + V3D(0, 0, (double)i / 300.0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_2 + V3D(0, 0, (double)i / 300.0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_3 + V3D(0, 0, (double)i / 300.0))));
    }

    for (int i = -50; i <= 50; i++) {
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_1 + V3D(0, (double)i / 147.0, 0))));
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_2 + V3D(0, (double)i / 147.0, 0))));
    }

    for (int i = -25; i <= 25; i++) {
      event_Qs.emplace_back(std::make_pair(
          std::make_pair(1., 1.), V3D(peak_1 + V3D(0, 0, (double)i / 61.0))));
    }

    double radius = 0.3;
    double radius_s = 0.1;
    int maxOrder = 1;
    bool crossTerm = false;
    Integrate3DEvents integrator(peak_q_list, hkl_list, mnp_list, UBinv, ModHKL,
                                 radius, radius_s, maxOrder, crossTerm);

    integrator.addEvents(event_Qs, false);

    // With fixed size ellipsoids, all the
    // events are counted.
    bool specify_size = true;
    double peak_radius = 0.3;
    double back_inner_radius = 0.3;
    double back_outer_radius = 0.35;
    std::vector<double> new_sigma;
    std::vector<Kernel::V3D> E1Vec;
    double inti;
    double sigi;
    for (size_t i = 0; i < peak_q_list.size(); i++) {
      V3D hkl = hkl_list[i];
      V3D mnp = mnp_list[i];
      auto shape = integrator.ellipseIntegrateModEvents(
          E1Vec, peak_q_list[i].second, hkl, mnp, specify_size, peak_radius,
          back_inner_radius, back_outer_radius, new_sigma, inti, sigi);
      TS_ASSERT_DELTA(inti, inti_all[i], 0.1);
      TS_ASSERT_DELTA(sigi, sigi_all[i], 0.01);

      auto ellipsoid_shape = boost::dynamic_pointer_cast<
          const Mantid::DataObjects::PeakShapeEllipsoid>(shape);
      TSM_ASSERT("Expect to get back an ellipsoid shape", ellipsoid_shape);
    }

    // The test data is not normally distributed,
    // so with 3 sigma half-axis sizes, we miss
    // some counts
    specify_size = false;
    for (size_t i = 0; i < peak_q_list.size(); i++) {
      V3D hkl = hkl_list[i];
      V3D mnp = mnp_list[i];
      integrator.ellipseIntegrateModEvents(
          E1Vec, peak_q_list[i].second, hkl, mnp, specify_size, peak_radius,
          back_inner_radius, back_outer_radius, new_sigma, inti, sigi);
      TS_ASSERT_DELTA(inti, inti_some[i], 0.1);
      TS_ASSERT_DELTA(sigi, sigi_some[i], 0.01);
    }
  }

  void test_integrateWeakPeakInPerfectCase() {
    /* Check that we can integrate a weak peak using a strong peak in the
     * perfect case when there is absolutely no background
     */

    // synthesize two peaks
    V3D peak_1(20, 0, 0);
    V3D peak_2(0, 20, 0);
    V3D peak_3(0, 0, 20);
    std::vector<std::pair<std::pair<double, double>, V3D>> peak_q_list{
        {std::make_pair(1., 1.), peak_1},
        {std::make_pair(1., 1.), peak_2},
        {std::make_pair(1., 1.), peak_3}};

    // synthesize a UB-inverse to map
    DblMatrix UBinv(3, 3, false); // Q to h,k,l
    UBinv.setRow(0, V3D(.1, 0, 0));
    UBinv.setRow(1, V3D(0, .2, 0));
    UBinv.setRow(2, V3D(0, 0, .25));

    std::vector<std::pair<std::pair<double, double>, V3D>> event_Qs;
    const int numStrongEvents = 10000;
    const int numWeakEvents = 100;
    generatePeak(event_Qs, peak_1, 0.1, numStrongEvents, 1); // strong peak
    generatePeak(event_Qs, peak_2, 0.1, numWeakEvents, 1);   // weak peak
    generatePeak(event_Qs, peak_2, 0.1, 0, 1); // non-existant peak

    IntegrationParameters params;
    params.peakRadius = 1.0;
    params.backgroundInnerRadius = 1.0;
    params.backgroundOuterRadius = 1.2;
    params.regionRadius = 1.2;
    params.specifySize = true;

    // Create integraton region + events & UB
    Integrate3DEvents integrator(peak_q_list, UBinv, params.regionRadius);
    integrator.addEvents(event_Qs, false);

    double strong_inti = 0, strong_sigi = 0;
    auto result = integrator.integrateStrongPeak(params, peak_1, strong_inti,
                                                 strong_sigi);
    const auto shape =
        boost::dynamic_pointer_cast<const PeakShapeEllipsoid>(result.first);
    const auto frac = std::get<0>(result.second);

    // Check the fraction of the peak that is contained within a "standard core"
    // the standard core is just the sigma in each direction
    TS_ASSERT_DELTA(frac, 0.8369, 0.01);

    // Check the integrated intensity for a strong peak is exactly what we set
    // it to be when generating the peak
    TS_ASSERT_DELTA(strong_inti, numStrongEvents, 0.01);
    TS_ASSERT_DELTA(strong_sigi, 100, 0.1);

    // Now integrate weak peak using the parameters we got from the strong peak
    double weak_inti, weak_sigi;
    integrator.integrateWeakPeak(params, shape, result.second, peak_2,
                                 weak_inti, weak_sigi);

    // Check the integrated intensity for a weak peak is exactly what we set it
    // to be weighted by the fraction of strong peak contained in a standard
    // core. This is not exactly the same because of the weighting from the
    // strong peak
    TS_ASSERT_DELTA(weak_inti, 83.696, 0.5);
    TS_ASSERT_DELTA(weak_sigi, 0.403, 0.1);

    weak_inti = 0;
    weak_sigi = 0;
    integrator.integrateWeakPeak(params, shape, result.second, peak_3,
                                 weak_inti, weak_sigi);

    // Check the integrated intensity for a weak peak is exactly what we set it
    // to be weighted by the fraction of strong peak contained in a standard
    // core. This is not exactly the same because of the weighting from the
    // strong peak
    TS_ASSERT_DELTA(weak_inti, 0, 0.5);
    TS_ASSERT_DELTA(weak_sigi, 0, 0.1);
  }

  void test_integrateWeakPeakWithBackground() {
    /* Check that we can integrate a weak peak using a strong peak in the
     * perfect case when there is absolutely no background
     */

    // synthesize two peaks
    V3D peak_1(20, 0, 0);
    V3D peak_2(0, 20, 0);
    std::vector<std::pair<std::pair<double, double>, V3D>> peak_q_list{
        {std::make_pair(1., 1.), peak_1}, {std::make_pair(1., 1.), peak_2}};

    // synthesize a UB-inverse to map
    DblMatrix UBinv(3, 3, false); // Q to h,k,l
    UBinv.setRow(0, V3D(.1, 0, 0));
    UBinv.setRow(1, V3D(0, .2, 0));
    UBinv.setRow(2, V3D(0, 0, .25));

    std::vector<std::pair<std::pair<double, double>, V3D>> event_Qs;
    const int numStrongEvents = 10000;
    const int numWeakEvents = 100;
    generatePeak(event_Qs, peak_1, 0.1, numStrongEvents, 1); // strong peak
    generatePeak(event_Qs, peak_2, 0.1, numWeakEvents, 1);   // weak peak
    generateUniformBackground(event_Qs, 10, -30, 30);

    IntegrationParameters params;
    params.peakRadius = 0.5;
    params.backgroundInnerRadius = 0.5;
    params.backgroundOuterRadius = 0.8;
    params.regionRadius = 0.5;

    // Create integraton regions + events & UB
    Integrate3DEvents integrator(peak_q_list, UBinv, params.regionRadius);
    integrator.addEvents(event_Qs, false);

    double strong_inti, strong_sigi;
    auto result = integrator.integrateStrongPeak(params, peak_1, strong_inti,
                                                 strong_sigi);
    const auto shape =
        boost::dynamic_pointer_cast<const PeakShapeEllipsoid>(result.first);
    const auto frac = std::get<0>(result.second);

    // Check the fraction of the peak that is contained within a "standard core"
    // the standard core is just the sigma in each direction
    TS_ASSERT_DELTA(frac, 0.8284, 0.01);

    // Check the integrated intensity for a strong peak is close to what we set
    // it to be when generating the peak
    TS_ASSERT_DELTA(strong_inti, numStrongEvents, 600);
    TS_ASSERT_DELTA(strong_sigi, 100, 0.1);

    // Now integrate weak peak using the parameters we got from the strong peak
    double weak_inti, weak_sigi;
    integrator.integrateWeakPeak(params, shape, result.second, peak_2,
                                 weak_inti, weak_sigi);

    // Check the integrated intensity for a weak peak is exactly what we set it
    // to be weighted by the fraction of strong peak contained in a standard
    // core. This is not exactly the same because of the weighting from the
    // strong peak
    TS_ASSERT_DELTA(weak_inti, numWeakEvents, 35);
    TS_ASSERT_DELTA(weak_sigi, 0.445, 0.2);
  }

  void test_estimateSignalToNoiseRatioInPerfectCase() {
    V3D peak_1(20, 0, 0);
    V3D peak_2(0, 20, 0);
    V3D peak_3(0, 0, 20);
    std::vector<std::pair<std::pair<double, double>, V3D>> peak_q_list{
        {std::make_pair(1., 1.), peak_1},
        {std::make_pair(1., 1.), peak_2},
        {std::make_pair(1., 1.), peak_3}};

    // synthesize a UB-inverse to map
    DblMatrix UBinv(3, 3, false); // Q to h,k,l
    UBinv.setRow(0, V3D(.1, 0, 0));
    UBinv.setRow(1, V3D(0, .2, 0));
    UBinv.setRow(2, V3D(0, 0, .25));

    std::vector<std::pair<std::pair<double, double>, V3D>> event_Qs;
    const int numStrongEvents = 10000;
    const int numWeakEvents = 100;
    generatePeak(event_Qs, peak_1, 0.1, numStrongEvents, 1);   // strong peak
    generatePeak(event_Qs, peak_2, 0.1, numWeakEvents, 1);     // weak peak
    generatePeak(event_Qs, peak_3, 0.1, numWeakEvents / 2, 1); // very weak peak

    // Create integraton region + events & UB
    Integrate3DEvents integrator(peak_q_list, UBinv, 1.5);
    integrator.addEvents(event_Qs, false);

    IntegrationParameters params;
    params.peakRadius = 0.5;
    params.backgroundInnerRadius = 0.5;
    params.backgroundOuterRadius = 0.8;
    params.regionRadius = 0.5;
    params.specifySize = true;

    const auto ratio1 = integrator.estimateSignalToNoiseRatio(params, peak_1);
    const auto ratio2 = integrator.estimateSignalToNoiseRatio(params, peak_2);
    const auto ratio3 = integrator.estimateSignalToNoiseRatio(params, peak_3);

    TS_ASSERT_DELTA(ratio1, numStrongEvents / 100, 1e-4);
    TS_ASSERT_DELTA(ratio2, numWeakEvents / 10, 1e-4);
    TS_ASSERT_DELTA(ratio3, 7.071, 1e-4);
  }

  void test_estimateSignalToNoiseRatioWithBackgroundAndOnePercentCulling() {
    doTestSignalToNoiseRatio(true, 0.05);
  }

  void test_estimateSignalToNoiseRatioWithBackgroundAndNoOnePercentCulling() {
    doTestSignalToNoiseRatio(false, 0.05);
  }

private:
  void doTestSignalToNoiseRatio(const bool useOnePercentBackgroundCorrection,
                                const double expectedFractionalDifference) {
    V3D peak_1(20, 0, 0);
    V3D peak_2(0, 20, 0);
    V3D peak_3(0, 0, 20);
    std::vector<std::pair<std::pair<double, double>, V3D>> peak_q_list{
        {std::make_pair(1., 1.), peak_1},
        {std::make_pair(1., 1.), peak_2},
        {std::make_pair(1., 1.), peak_3}};

    // synthesize a UB-inverse to map
    DblMatrix UBinv(3, 3, false); // Q to h,k,l
    UBinv.setRow(0, V3D(.1, 0, 0));
    UBinv.setRow(1, V3D(0, .2, 0));
    UBinv.setRow(2, V3D(0, 0, .25));

    // set integration parameters
    IntegrationParameters params;
    params.peakRadius = 0.5;
    params.backgroundInnerRadius = 0.5;
    params.backgroundOuterRadius = 0.8;
    params.regionRadius = 0.5;
    params.specifySize = true;
    // calculate ratio of background shell to peak region
    const auto volRatio =
        pow(params.peakRadius, 3) / (pow(params.backgroundOuterRadius, 3) -
                                     pow(params.backgroundInnerRadius, 3));

    std::vector<std::pair<std::pair<double, double>, V3D>> event_Qs;
    const int nPointsBg = 100000;   // counts in backgroundOuterRadius
    const double ctsPerBgEvent = 1; // counts per bg event
    const auto nBgEventsInPeakRegion =
        nPointsBg *
        (pow(params.peakRadius, 3) / pow(params.backgroundOuterRadius, 3));
    const auto nBgEventsInShell = nBgEventsInPeakRegion / volRatio;

    std::vector<double> fracEvents{10, 1,
                                   0.1}; // fraction of nBgEventsInPeakRegion
    std::vector<int> nPeakEvents;
    double ctsPerPeakEvent = 2;
    for (size_t i = 0; i < peak_q_list.size(); i++) {
      nPeakEvents.push_back(static_cast<int>(
          ceil(fracEvents[i] * static_cast<double>(nBgEventsInPeakRegion))));
      generatePeak(event_Qs, peak_q_list[i].second, 0.08, nPeakEvents.back(),
                   ctsPerPeakEvent);
      generateUniformBackgroundSpherical(
          event_Qs, nPointsBg, peak_q_list[i].second,
          params.backgroundOuterRadius, ctsPerBgEvent);
    }

    // Create integraton region + events & UB
    Integrate3DEvents integrator(peak_q_list, UBinv, 1.5,
                                 useOnePercentBackgroundCorrection);
    integrator.addEvents(event_Qs, false);

    // calculate signal-noise ratio
    std::vector<double> fractionalDiff(peak_q_list.size());

    for (size_t i = 0; i < peak_q_list.size(); i++) {

      auto ratio = integrator.estimateSignalToNoiseRatio(
          params, peak_q_list[i].second, true, 0.15);

      // calculate the expected value
      double ratioExpected;
      if (useOnePercentBackgroundCorrection) {
        auto sigi =
            sqrt(nBgEventsInPeakRegion * ctsPerBgEvent * 0.99 +
                 nPeakEvents[i] * ctsPerPeakEvent +
                 pow(volRatio, 2) * nBgEventsInShell * ctsPerBgEvent * 0.99);
        auto inti = nPeakEvents[i] * ctsPerPeakEvent +
                    0.01 * nBgEventsInPeakRegion * ctsPerBgEvent;
        ratioExpected = inti / sigi;
      } else {
        auto sigi = sqrt(nBgEventsInPeakRegion * ctsPerBgEvent +
                         nPeakEvents[i] * ctsPerPeakEvent +
                         pow(volRatio, 2) * nBgEventsInShell * ctsPerBgEvent);
        ratioExpected = (nPeakEvents[i] * ctsPerPeakEvent) / sigi;
      }

      fractionalDiff[i] = abs(ratio - ratioExpected) / ratioExpected;
    }
    TS_ASSERT_LESS_THAN(fractionalDiff[0], expectedFractionalDifference)
    TS_ASSERT_LESS_THAN(fractionalDiff[1], expectedFractionalDifference)
    TS_ASSERT_LESS_THAN(fractionalDiff[2], expectedFractionalDifference)
  }

  /** Generate a symmetric Gaussian peak
   *
   * @param event_Qs :: vector of event Qs
   * @param center :: location of the center of the peak
   * @param sigma :: standard deviation of the peak
   * @param numSamples :: number of samples to draw
   * @param seed :: the seed to the pseudo-random number generator
   */
  void
  generatePeak(std::vector<std::pair<std::pair<double, double>, V3D>> &event_Qs,
               V3D center, double sigma = 5, size_t numSamples = 1000,
               double scale = 1.0, int seed = 1) {

    std::mt19937 gen;
    std::normal_distribution<> d(0, sigma);
    gen.seed(seed);

    for (size_t i = 0; i < numSamples; ++i) {
      V3D offset(d(gen), d(gen), d(gen));
      event_Qs.emplace_back(
          std::make_pair(std::make_pair(scale, scale), center + offset));
    }
  }

  /** Generate a uniform background
   *
   * @param event_Qs :: vector of event Qs
   * @param countsPerQ :: average value for the flat background
   * @param lower :: the smallest extent of Q space in all directions
   * @param upper :: the largest extent of Q space in all directions
   * @param countVariation :: how much the average background can vary by
   * @param seed :: the random seed to use (default 1)
   */
  void generateUniformBackground(
      std::vector<std::pair<std::pair<double, double>, V3D>> &event_Qs,
      size_t countsPerQ, const double lower, const double upper,
      const int countVariation = 3, const double step = 0.5, int seed = 1) {
    const auto counts = static_cast<double>(countsPerQ);
    std::mt19937 gen;
    std::uniform_real_distribution<> d(-countVariation, countVariation);
    gen.seed(seed);

    for (double i = lower; i < upper; i += step) {
      for (double j = lower; j < upper; j += step) {
        for (double k = lower; k < upper; k += step) {
          double cts = counts + d(gen);
          event_Qs.emplace_back(std::make_pair(cts, cts), V3D(i, j, k));
        }
      }
    }
  }
  /** Generate a randomly distributed uniform background in sphere
   ** so as to maximise number of points relevant to integration
   * @param event_Qs :: vector of event Qs
   * @param npts :: number of events
   * @param center :: center of sphere
   * @param radius :: radius of sphere
   * @param scale :: signal per event
   * @param seed :: the random seed to use (default 1)
   */
  void generateUniformBackgroundSpherical(
      std::vector<std::pair<std::pair<double, double>, V3D>> &event_Qs,
      int npts, V3D center, const double radius = 1, const double scale = 1,
      int seed = 1) {

    std::mt19937 gen;
    std::uniform_real_distribution<> d(-radius, radius);
    gen.seed(seed);

    int n = 0;
    do {
      V3D point(d(gen), d(gen), d(gen));
      if (point.norm() <= radius) {
        event_Qs.emplace_back(std::make_pair(scale, scale), center + point);
        n++;
      }
    } while (n < npts);
  }
};

#endif /* MANTID_MDEVENTS_INTEGRATE_3D_EVENTS_TEST_H_ */
