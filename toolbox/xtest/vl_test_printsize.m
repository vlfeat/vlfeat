classdef vl_test_printsize < matlab.unittest.TestCase
  properties
    fig
    usletter
    a4
  end
  
  methods (TestClassSetup)
    function ssetup(t)
      t.fig = figure(1) ;
      t.usletter = [8.5, 11] ; % inches
      t.a4 = [8.26772, 11.6929] ;
      clf(t.fig) ; plot(1:10) ;
    end
  end
  
  methods (TestClassTeardown)
    function teardown(t)      
      close(t.fig) ;
    end
  end
  
  methods (Test)
    function test_basic(t)
      for sigma = [1 0.5 0.2]
        vl_printsize(t.fig, sigma) ;
        set(1, 'PaperUnits', 'inches') ;
        siz = get(1, 'PaperSize') ;
        pos = get(1, 'PaperPosition') ;
        t.verifyEqual(siz(1), sigma*t.usletter(1), 'AbsTol', 1e-4) ;
        t.verifyEqual(pos(1), 0, 'AbsTol', 1e-4) ;
        t.verifyEqual(pos(3), sigma*t.usletter(1), 'AbsTol', 1e-4) ;
      end
    end
    
    function test_papertype(t)
      vl_printsize(t.fig, 1, 'papertype', 'a4') ;
      set(1, 'PaperUnits', 'inches') ;
      siz = get(1, 'PaperSize') ;
      pos = get(1, 'PaperPosition') ;
      t.verifyEqual(siz(1), t.a4(1), 'AbsTol', 1e-4) ;
    end
    
    function test_margin(t)
      m = 0.5 ;
      vl_printsize(t.fig, 1, 'margin', m) ;
      set(1, 'PaperUnits', 'inches') ;
      siz = get(1, 'PaperSize') ;
      pos = get(1, 'PaperPosition') ;
      t.verifyEqual(siz(1), t.usletter(1) * (1 + 2*m), 'AbsTol', 1e-4) ;
      t.verifyEqual(pos(1), t.usletter(1) * m, 'AbsTol', 1e-4) ;
    end
    
    function test_reference(t)
      sigma = 1 ;
      vl_printsize(t.fig, 1, 'reference', 'vertical') ;
      set(1, 'PaperUnits', 'inches') ;
      siz = get(1, 'PaperSize') ;
      pos = get(1, 'PaperPosition') ;
      t.verifyEqual(siz(2), sigma*t.usletter(2), 'AbsTol', 1e-4) ;
      t.verifyEqual(pos(2), 0, 'AbsTol', 1e-4) ;
      t.verifyEqual(pos(4), sigma*t.usletter(2), 'AbsTol', 1e-4) ;
    end
  end
end






