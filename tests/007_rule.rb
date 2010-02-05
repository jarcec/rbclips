require 'test/unit'
require 'rbclips'

class Test_Rule < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Rule)
    assert Clips::Rule.constants.member?(:Creator)
  end

  def test_new_block
    assert_raise(Clips::UsageError)     { Clips::Rule::Creator.new }
    assert_raise(Clips::ArgumentError)  { Clips::Rule.new(3){}  }
  end
end
