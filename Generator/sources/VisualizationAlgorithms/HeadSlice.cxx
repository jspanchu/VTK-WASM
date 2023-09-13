#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkExtractVOI.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " FullHead.mhd" << std::endl;
    return EXIT_FAILURE;
  }

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> ren1;

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // create pipeline
  //

  vtkNew<vtkMetaImageReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkExtractVOI> extractVOI;
  extractVOI->SetInputConnection(reader->GetOutputPort());
  extractVOI->SetVOI(0, 255, 0, 255, 45, 45);

  vtkNew<vtkContourFilter> iso;
  iso->SetInputConnection(extractVOI->GetOutputPort());
  iso->GenerateValues(12, 500, 1150);

  vtkNew<vtkPolyDataMapper> isoMapper;
  isoMapper->SetInputConnection(iso->GetOutputPort());
  isoMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> isoActor;
  isoActor->SetMapper(isoMapper);
  isoActor->GetProperty()->SetColor(colors->GetColor3d("Wheat").GetData());

  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputConnection(extractVOI->GetOutputPort());

  vtkNew<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outline->GetOutputPort());

  vtkNew<vtkActor> outlineActor;
  outlineActor->SetMapper(outlineMapper);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(outlineActor);
  ren1->AddActor(isoActor);
  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(1.5);
  ren1->ResetCameraClippingRange();

  renWin->SetSize(640, 640);
  renWin->SetWindowName("HeadSlice");

  renWin->Render();
  iren->Start();
  return EXIT_SUCCESS;
}
