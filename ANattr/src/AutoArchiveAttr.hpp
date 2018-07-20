#ifndef AUTOARCHIVEATTR_HPP_
#define AUTOARCHIVEATTR_HPP_
//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #9 $
//
// Copyright 2009-2017 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
// Description :
//============================================================================

#include "TimeSlot.hpp"

namespace ecf { class Calendar;} // forward declare class

namespace ecf {

// Use compiler ,  destructor, assignment, copy constructor
class AutoArchiveAttr  {
public:
   AutoArchiveAttr() : relative_(true),days_(false) {}
   AutoArchiveAttr(int hour, int minute, bool relative ) : time_(hour, minute), relative_(relative), days_(false) {}
   AutoArchiveAttr(const TimeSlot& ts,   bool relative ) : time_(ts),           relative_(relative), days_(false) {}
   AutoArchiveAttr(int days) : time_( TimeSlot(days*24,0) ), relative_(true), days_(true) {}

   std::ostream& print(std::ostream&) const;
   bool operator==(const AutoArchiveAttr& rhs) const;
   bool isFree(const ecf::Calendar&, const boost::posix_time::time_duration& suiteDurationAtComplete) const;

   std::string toString() const;

   const TimeSlot& time() const { return time_;}
   bool relative() const { return relative_; }
   bool days() const { return days_; }

private:
   TimeSlot time_;
   bool relative_;
   bool days_;

   friend class cereal::access;
   template<class Archive>
   void serialize(Archive & ar, std::uint32_t const version )
   {
      ar( CEREAL_NVP(time_));
      CEREAL_OPTIONAL_NVP(ar, relative_, [this](){return !relative_;}); // conditionally save
      CEREAL_OPTIONAL_NVP(ar, days_,     [this](){return days_; });     // conditionally save
   }
};

}
#endif