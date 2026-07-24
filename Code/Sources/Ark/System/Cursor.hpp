#pragma once

#include "Ark/Core/Core.hpp"

namespace Ark::System
{
    /// System cursor types matching common platform cursors
    enum class SystemCursor
    {
        Arrow,     // Default cursor. Usually an arrow.
        IBeam,     // Text selection. Usually an I-beam.
        Wait,      // Wait. Usually an hourglass or watch or spinning ball.
        Crosshair, // Crosshair.
        WaitArrow, // Program is busy but still interactive. Usually WAIT with an arrow.
        SizeNWSE,  // Double arrow pointing northwest and southeast.
        SizeNESW,  // Double arrow pointing northeast and southwest.
        SizeWE,    // Double arrow pointing west and east.
        SizeNS,    // Double arrow pointing north and south.
        SizeAll,   // Four pointed arrow pointing north, south, east, and west.
        No,        // Not permitted. Usually a slashed circle or crossbones.
        Hand,      // Pointer that indicates a link. Usually a pointing hand.
    };

    /// Cursor management functions
    namespace Cursor
    {
        /// Show or hide the system cursor
        void setVisible(bool visible);

        /// Check if the system cursor is currently visible
        bool isVisible();

        /// Set the system cursor to one of the predefined cursor types
        /// @param cursor The system cursor type to display
        void setCursor(SystemCursor cursor);

        /// Reset the cursor to the default arrow cursor
        void resetCursor();
    }
}
