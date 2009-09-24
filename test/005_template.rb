require 'test/unit'
require 'rbclips'

class Test_Template < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Template)
  end
end
