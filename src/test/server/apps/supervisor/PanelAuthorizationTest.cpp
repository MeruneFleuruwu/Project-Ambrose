/*
 * Project Ambrose by Imjustchico
 * Tests the order a request is decided in, which matters more than any single rule: a role answers first, a grant only ever adds and never takes away, somebody holding nothing on an app is told there is no such app rather than that they may not touch it, so a refusal is never a list of what exists, and a permission the catalog does not hold is refused rather than quietly allowed. Also that the app a request is about is read from its path, since everything above depends on knowing which app is being asked for.
 */

#include "PanelAuthorization.h"
#include "PanelGrants.h"
#include "PanelPermissions.h"

#include <gtest/gtest.h>

#include <string>

namespace
{
    PanelAsking As(PanelRole role)
    {
        PanelAsking asking;
        asking.UserId = 7;
        asking.Role = role;
        asking.IsOwner = role == PanelRole::Owner;
        return asking;
    }
}

TEST(PanelAuthorizationTest, TheAppBeingAskedAboutIsReadFromThePath)
{
    EXPECT_EQ(PanelAuthorization::AppInPath("/api/apps/gameserver/api/command"), "gameserver");
    EXPECT_EQ(PanelAuthorization::AppInPath("/api/apps/loginserver"), "loginserver");
    EXPECT_EQ(PanelAuthorization::AppInPath("/api/apps/"), "");
    EXPECT_EQ(PanelAuthorization::AppInPath("/api/status"), "") << "a request about no app in particular names none";
    EXPECT_EQ(PanelAuthorization::AppInPath("/api/panel/users"), "");
}

TEST(PanelAuthorizationTest, ARoleAnswersFirst)
{
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Owner), "accounts.delete", false, false, true), PermissionVerdict::Allowed);
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Operator), "console.write", false, false, true), PermissionVerdict::Allowed);
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Viewer), "console.read", false, false, true), PermissionVerdict::Allowed);
}

TEST(PanelAuthorizationTest, AGrantAddsAndNeverTakesAway)
{
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Viewer), "power.restart", false, true, true), PermissionVerdict::Forbidden)
        << "holding something else on the app is not holding this";
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Viewer), "power.restart", true, true, true), PermissionVerdict::Allowed)
        << "the grant is what allows it";

    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Operator), "console.write", false, false, true), PermissionVerdict::Allowed)
        << "a role that already allows it does not need a grant, and the absence of one takes nothing away";
}

TEST(PanelAuthorizationTest, HoldingNothingOnAnAppMeansThereIsNoSuchApp)
{
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Viewer), "power.restart", false, false, true), PermissionVerdict::OutOfScope)
        << "a refusal must not double as a list of the apps that exist";
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Viewer), "power.restart", false, true, true), PermissionVerdict::Forbidden)
        << "somebody who holds something here already knows the app is there";
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Viewer), "panel.settings", false, false, false), PermissionVerdict::Forbidden)
        << "a request about no app in particular cannot be out of scope";
}

TEST(PanelAuthorizationTest, AnOwnerOnlyPermissionIsNotAnAdministratorsAndAnUnknownOneIsNobodys)
{
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Admin), "files.roots", false, false, false), PermissionVerdict::Forbidden);
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Owner), "files.roots", false, false, false), PermissionVerdict::Allowed);
    EXPECT_FALSE(PanelPermissions::Holds("console.everything"));
    EXPECT_EQ(PanelAuthorization::Weigh(As(PanelRole::Owner), "console.everything", false, false, false), PermissionVerdict::Forbidden)
        << "even the owner cannot hold what the catalog does not name";
}
