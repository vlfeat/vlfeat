function results = vl_test_printsize(varargin)
% VL_TEST_PRINTSIZE
vl_test_init ;

function s = setup()
s.fig = figure(1) ;
s.usletter = [8.5, 11] ; % inches
s.a4 = [8.26772, 11.6929] ;
clf(s.fig) ; plot(1:10) ;

function teardown(s)
close(s.fig) ;

function test_basic(s)
for sigma = [1 0.5 0.2]
  vl_printsize(s.fig, sigma) ;
  set(1, 'PaperUnits', 'inches') ;
  siz = get(1, 'PaperSize') ;
  pos = get(1, 'PaperPosition') ;
  vl_assert_almost_equal(siz(1), sigma*s.usletter(1), 1e-4) ;
  vl_assert_almost_equal(pos(1), 0, 1e-4) ;
  vl_assert_almost_equal(pos(3), sigma*s.usletter(1), 1e-4) ;
end

function test_papertype(s)
vl_printsize(s.fig, 1, 'papertype', 'a4') ;
set(1, 'PaperUnits', 'inches') ;
siz = get(1, 'PaperSize') ;
pos = get(1, 'PaperPosition') ;
vl_assert_almost_equal(siz(1), s.a4(1), 1e-4) ;

function test_margin(s)
m = 0.5 ;
vl_printsize(s.fig, 1, 'margin', m) ;
set(1, 'PaperUnits', 'inches') ;
siz = get(1, 'PaperSize') ;
pos = get(1, 'PaperPosition') ;
vl_assert_almost_equal(siz(1), s.usletter(1) * (1 + 2*m), 1e-4) ;
vl_assert_almost_equal(pos(1), s.usletter(1) * m, 1e-4) ;

function test_reference(s)
sigma = 1 ;
vl_printsize(s.fig, 1, 'reference', 'vertical') ;
set(1, 'PaperUnits', 'inches') ;
siz = get(1, 'PaperSize') ;
pos = get(1, 'PaperPosition') ;
vl_assert_almost_equal(siz(2), sigma*s.usletter(2), 1e-4) ;
vl_assert_almost_equal(pos(2), 0, 1e-4) ;
vl_assert_almost_equal(pos(4), sigma*s.usletter(2), 1e-4) ;
