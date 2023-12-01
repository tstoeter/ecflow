/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_node_parser_DefsParser_HPP
#define ecflow_node_parser_DefsParser_HPP

#include "ecflow/node/parser/Parser.hpp"

class DefsParser : public Parser {
public:
    explicit DefsParser(DefsStructureParser* p);
    DefsParser(DefsStructureParser* p, bool node_parser_only);
    const char* keyword() const override { return "DEFS"; }
};

#endif /* ecflow_node_parser_DefsParser_HPP */