#ifndef OSMIUM_GEOM_WKT_HPP
#define OSMIUM_GEOM_WKT_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013-2017 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/factory.hpp>

namespace osmium {

    namespace geom {

        enum class wkt_type : bool {
            wkt  = false,
            ewkt = true
        }; // enum class wkt_type

        namespace detail {

            class WKTFactoryImpl {

                std::string m_srid_prefix;
                std::string m_str;
                int m_precision;
                wkt_type m_wkt_type;

            public:

                using point_type        = std::string;
                using linestring_type   = std::string;
                using polygon_type      = std::string;
                using multipolygon_type = std::string;
                using ring_type         = std::string;

                WKTFactoryImpl(int srid, int precision = 7, wkt_type wtype = wkt_type::wkt) :
                    m_srid_prefix(),
                    m_precision(precision),
                    m_wkt_type(wtype) {
                    if (m_wkt_type == wkt_type::ewkt) {
                        m_srid_prefix = "SRID=";
                        m_srid_prefix += std::to_string(srid);
                        m_srid_prefix += ';';
                    }
                }

                /* Point */

                point_type make_point(const osmium::geom::Coordinates& xy) const {
                    std::string str {m_srid_prefix};
                    str += "POINT";
                    xy.append_to_string(str, '(', ' ', ')', m_precision);
                    return str;
                }

                /* LineString */

                void linestring_start() {
                    m_str = m_srid_prefix;
                    m_str += "LINESTRING(";
                }

                void linestring_add_location(const osmium::geom::Coordinates& xy) {
                    xy.append_to_string(m_str, ' ', m_precision);
                    m_str += ',';
                }

                linestring_type linestring_finish(size_t /* num_points */) {
                    assert(!m_str.empty());
                    std::string str;

                    using std::swap;
                    swap(str, m_str);

                    str.back() = ')';
                    return str;
                }

                /* MultiPolygon */

                void multipolygon_start() {
                    m_str = m_srid_prefix;
                    m_str += "MULTIPOLYGON(";
                }

                void multipolygon_polygon_start() {
                    m_str += '(';
                }

                void multipolygon_polygon_finish() {
                    m_str += "),";
                }

                void multipolygon_outer_ring_start() {
                    m_str += '(';
                }

                void multipolygon_outer_ring_finish() {
                    assert(!m_str.empty());
                    m_str.back() = ')';
                }

                void multipolygon_inner_ring_start() {
                    m_str += ",(";
                }

                void multipolygon_inner_ring_finish() {
                    assert(!m_str.empty());
                    m_str.back() = ')';
                }

                void multipolygon_add_location(const osmium::geom::Coordinates& xy) {
                    xy.append_to_string(m_str, ' ', m_precision);
                    m_str += ',';
                }

                multipolygon_type multipolygon_finish() {
                    assert(!m_str.empty());
                    std::string str;

                    using std::swap;
                    swap(str, m_str);

                    str.back() = ')';
                    return str;
                }

            }; // class WKTFactoryImpl

        } // namespace detail

        template <typename TProjection = IdentityProjection>
        using WKTFactory = GeometryFactory<osmium::geom::detail::WKTFactoryImpl, TProjection>;

    } // namespace geom

} // namespace osmium

#endif // OSMIUM_GEOM_WKT_HPP
