using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Clipper2Lib.UnitTests
{
  [TestClass]
  public class TestCompareWithCPP
  {
    [TestMethod]
    public void TestCompare()
    {
      Paths64 subject = new(), subjectOpen = new(), clip = new();

      Assert.IsTrue(ClipperFileIO.LoadTestNum("..\\..\\..\\..\\..\\..\\Tests\\CPP-vs-CS.txt",
        1, subject, subjectOpen, clip, out ClipType cliptype, out FillRule fillrule,
        out _, out _, out _),
          "Unable to read PolytreeHoleOwner2.txt");

      PolyTree64 solutionTree = new();
      Paths64 solution_open = new();
      Clipper64 clipper = new();

      clipper.AddSubject(subject);
      clipper.AddOpenSubject(subjectOpen);
      clipper.AddClip(clip);
      clipper.Execute(cliptype, fillrule, solutionTree, solution_open);

      Paths64 solutionPaths = Clipper.PolyTreeToPaths64(solutionTree);
      double a1 = Clipper.Area(solutionPaths), a2 = solutionTree.Area();

      Assert.IsTrue(a1 == 846.5,
        string.Format("solution has wrong area - value expected: 107,100; value returned; {0} ", a1));

      Assert.IsTrue(a1 == a2,
        string.Format("solution tree has wrong area - value expected: {0}; value returned; {1} ", a1, a2));
    }
  }
}
