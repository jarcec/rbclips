require 'test/unit'
require 'rbclips'

class Test_Base < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Environment)
  end
end
