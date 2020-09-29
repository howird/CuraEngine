// Copyright (c) 2020 Ultimaker B.V.
// CuraEngine is released under the terms of the AGPLv3 or higher.

#include "WallToolPaths.h"

#include "SkeletalTrapezoidation.h"
#include "utils/polygonUtils.h"

namespace cura
{
constexpr coord_t transition_length_multiplier = 2;

WallToolPaths::WallToolPaths(const Polygons& outline, const coord_t nominal_bead_width, const coord_t inset_count,
                             const Settings& settings)
    : outline(outline)
    , bead_width_0(nominal_bead_width)
    , bead_width_x(nominal_bead_width)
    , inset_count(inset_count)
    , strategy_type(settings.get<StrategyType>("beading_strategy_type"))
    , widening_beading_enabled(settings.get<bool>("widening_beading_enabled"))
    , min_bead_width(widening_beading_enabled ? new coord_t(settings.get<coord_t>("min_bead_width")) : nullptr)
    , min_feature_size(widening_beading_enabled ? new coord_t(settings.get<coord_t>("min_feature_size")) : nullptr)
    , small_area_length(INT2MM(static_cast<double>(nominal_bead_width) / 2))
    , transition_length(transition_length_multiplier * nominal_bead_width)
    , toolpaths_generated(false)
{
}

WallToolPaths::WallToolPaths(const Polygons& outline, const coord_t bead_width_0, const coord_t bead_width_x,
                             const coord_t inset_count, const Settings& settings)
    : outline(outline)
    , bead_width_0(bead_width_0)
    , bead_width_x(bead_width_x)
    , inset_count(inset_count)
    , strategy_type(settings.get<StrategyType>("beading_strategy_type"))
    , widening_beading_enabled(settings.get<bool>("widening_beading_enabled"))
    , min_bead_width(widening_beading_enabled ? new coord_t(settings.get<coord_t>("min_bead_width")) : nullptr)
    , min_feature_size(widening_beading_enabled ? new coord_t(settings.get<coord_t>("min_feature_size")) : nullptr)
    , small_area_length(INT2MM(static_cast<double>(bead_width_0) / 2))
    , transition_length(transition_length_multiplier * bead_width_0)
    , toolpaths_generated(false)
{
}

const VariableWidthPaths& WallToolPaths::generate()
{
    assert(("inset count should be more then 0", inset_count > 0));
    constexpr coord_t smallest_segment = 50;
    constexpr coord_t allowed_distance = 50;
    constexpr coord_t epsilon_offset = (allowed_distance / 2) - 1;
    constexpr float transitioning_angle = 0.5;

    // Simplify outline for boost::voronoi consumption. Absolutely no self intersections or near-self instersections allowed:
    // TODO: Open question: Does this indeed fix all (or all-but-one-in-a-million) cases for manifold but otherwise possibly complex polygons?
    Polygons prepared_outline = outline.offset(-epsilon_offset).offset(epsilon_offset);
    prepared_outline.simplify(smallest_segment, allowed_distance);
    PolygonUtils::fixSelfIntersections(epsilon_offset, prepared_outline);
    prepared_outline.removeDegenerateVerts();
    prepared_outline.removeColinearEdges();
    prepared_outline.removeSmallAreas(small_area_length * small_area_length, false);

    if (prepared_outline.area() > 0)
    {
        const coord_t max_bead_count = 2 * inset_count;
        const auto beading_strat = std::unique_ptr<BeadingStrategy>(BeadingStrategyFactory::makeStrategy(
            strategy_type, bead_width_0, bead_width_x, transition_length, transitioning_angle, min_bead_width,
            min_feature_size, max_bead_count));
        SkeletalTrapezoidation wall_maker(prepared_outline, *beading_strat, beading_strat->transitioning_angle);
        wall_maker.generateToolpaths(toolpaths);
    }
    removeEmptyToolPaths(toolpaths);
    toolpaths_generated = true;
    return toolpaths;
}

const VariableWidthPaths& WallToolPaths::getToolPaths()
{
    if (!toolpaths_generated)
    {
        return generate();
    }
    return toolpaths;
}

const Polygons& WallToolPaths::getInnerContour()
{
    if (!toolpaths_generated && inset_count > 0)
    {
        generate();
    }
    else
    {
        return outline;
    }
    // TODO: CURA-7681  -> inner_contour = innerContourFromToolpaths(toolpaths);
    // TODO: Check to make sure if this "correctly generated for now"
    if (inner_contour.empty())
    {
        const coord_t offset_distance = bead_width_0 * inset_count;
        inner_contour = outline.offset(-static_cast<int>(offset_distance));
        inner_contour.simplify();
    }
    return inner_contour;
}

const Polygons& WallToolPaths::getOutline() const
{
    return outline;
}

Polygons WallToolPaths::innerContourFromToolpaths(const VariableWidthPaths& toolpaths)
{
    // TODO: CURA-7681
    return Polygons();
}

bool WallToolPaths::removeEmptyToolPaths(VariableWidthPaths& toolpaths)
{
    toolpaths.erase(std::remove_if(toolpaths.begin(), toolpaths.end(), [](const VariableWidthLines& lines)
                                   {
                                       return lines.empty();
                                   }), toolpaths.end());
    return toolpaths.empty();
}


} // namespace cura